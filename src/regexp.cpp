#include "regexp/regexp.hpp"
#include "deffer.hpp"

Regexp::Regexp(char *search, int vector_size)
    :
        vector_size(vector_size),
        pos(_property),
        string(_property),
        length(_property)
{
    vector = new int[vector_size];
    match  = nullptr;
    errstr = nullptr;
    stat_success = 0;
    stat_failed  = 0;

    if (search) {
        string = search;
    }

}

Regexp::~Regexp() {
    for (auto &regexp : regexps) {
        delete regexp.second;
    }
    if (match) {
        pcre_free_substring_list(match);
        match = nullptr;
    }
    delete[] vector;
}

Regexp::regexp_s::regexp_s() {
    regexp  = nullptr;
    regexpe = nullptr;
    keep_matched_pos = 0;
}

Regexp::regexp_s::~regexp_s() {

    if (regexp) {
        pcre_free(regexp);
        regexp = nullptr;
    }

    if (regexpe) {
        pcre_free_study(regexpe);
        regexpe = nullptr;
    }

}

int Regexp::operator()(const std::string &expression, const char *str) {

    if (match) {
        //free previous search
        pcre_free_substring_list(match);
        match = nullptr;
    }

    unless (regexps.count(expression)) {
        //add new one
        auto regexs = compile(expression.c_str());
        unless (regexs) {
            lf("failed compile regexp expression '{}'", expression);
            return ::error;
        }
        regexps[expression] = regexs;
    }
    ld(10, "pattern expression '{}'", expression);
    auto regexs = regexps[expression];
    affirm(regexs == nullptr and "regexp by pattern '{}' not found", expression);

    const char *search = (str == nullptr ? string : str);

    // either continue matched on the predefine string
    affirm((char *)search == nullptr and "string to search is undefined");

    ld(10, "regexp string '{}'", search);
    int rc = pcre_exec(
        regexs->regexp,
        regexs->regexpe,
        search,
        strlen(search),
        0,
        0,
        vector,
        vector_size
    );

    ld(10, "regexp result {}", rc);

    //FIXME I think most of the error messages need to be fatal instead of error!
    if(rc < 0) {
        stat_failed++;
        switch(rc) {
            case PCRE_ERROR_NOMATCH:
                return ::error;
            case PCRE_ERROR_NULL:
                le("something was nullptr");
                return ::error;
            case PCRE_ERROR_BADOPTION:
                le("bad option was passed");
                return ::error;
            case PCRE_ERROR_BADMAGIC:
                le("magic number bad (compiled re corrupt?)");
                return ::error;
            case PCRE_ERROR_UNKNOWN_NODE:
                le("something kooky in the compiled re");
                return ::error;
            case PCRE_ERROR_NOMEMORY:
                le("ran out of memory");
                return ::error;
            default:
                le("unknown error");
                return ::error;
        }
    }

    matched = rc;
    stat_success++;

    if (rc == 0) {
        ld(10, "match result zero, set to vec size {}", rc);
    }

    rc = pcre_get_substring_list(search, vector, matched, &match);
    if (rc) {
        le("regexp '{}' error get matched substrings, error {}", expression, rc);
        return ::error;
    }

    ld(10, "regexp {} matched {}", expression, matched);

    unless (str) {
        if (regexs->keep_matched_pos) {
            string += vector[1];
        }
    }
    return matched;
}

char *Regexp::operator[](unsigned int i) {
    if (i >= matched) {
        return nullptr;
    }
    return (char *)match[i];
}

bool Regexp::exists(unsigned int i) {
    if (i >= matched) {
        return false;
    }
    if (vector[i * 2 + 1] == -1) {
        return false;
    }
    return true;
}

int Regexp::index(unsigned int i, int whence) {     // whence 0 start, 1 end
    if (i >= matched) {
        return 0;
    }
    if (vector[i * 2 + 1] == -1) {
        return 0;
    }
    return vector[i * 2 + (whence ? 2 : 1)];
}

Regexp::regexp_t* Regexp::compile(const char *_expression) {

    ld(10, "compile regexp '{}'", _expression);

    char *expression = new char[strlen(_expression) + 1];
    deffer delete_expression([expression](){delete[] expression;});
    strcpy(expression, _expression);
    char * expression_p = expression + strlen(expression) - 1;

    unless ((char)*expression == '/') {
        le("wrong regexp pattern '{}', pattern must be placed between '/'", _expression);
        return nullptr;
    }

    int options = 0;
    int keep_matched_pos = 0;
    while (*expression_p != *expression) {
        ld(10, "expression option {}", *expression_p);
        switch (*expression_p) {
            case 'c':
                keep_matched_pos = 1;
                break;
            case 'i':
                options |= PCRE_CASELESS;
                break;
            case 'm':
                options |= PCRE_MULTILINE;
                break;
            case 's':
                options |= PCRE_DOTALL;
                break;
            case 'x':
                options |= PCRE_EXTENDED;
                break;
            case 'o':
            case 'g':
                //does not make sense for library, by default library worked as this options are set
                break;
            default:
                //le("unknown pattern modifier '{}'", *expression_p);
                break;
        }
        if (expression_p == expression) {
            le("wrong regexp patter '{}', patter must be placed between '/'", _expression);
            return nullptr;
        }
        expression_p--;
    }

    // remove // in pattern to provide pure regexp pattern for pcre_compile
    *expression_p = 0;
    expression++;

    regexp_t *regexs = new regexp_t;
    regexs->keep_matched_pos = keep_matched_pos;

    regexs->regexp = pcre_compile(expression, options, &errstr, &erroff, nullptr);
    unless (regexs->regexp) {
        le("failed compile regexp expression '{}'", _expression);
        delete regexs;
        return nullptr;
    }

    regexs->regexpe = pcre_study(regexs->regexp, 0, &errstr);
    if (errstr)
        lw("could not study regexp expression '{}': {}\n", _expression, errstr);

    int capture_count;
    pcre_fullinfo(regexs->regexp, regexs->regexpe, PCRE_INFO_CAPTURECOUNT, &capture_count);

    ld(10, "regexp compile '{}' stat_success, capture counts {}", _expression, capture_count);

    return regexs;
}

