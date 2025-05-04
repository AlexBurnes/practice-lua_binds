#pragma once
#ifndef REGEXP_H
#define REGEXP_H

extern "C" {
#include <pcre.h>
}

#include "sparsepp/spp.h"
#include "typedef.h"
#include "log/log.hpp"
#include <memory>

class Regexp {

    private:

        typedef struct regexp_s {
            pcre        *regexp;
            pcre_extra  *regexpe;
            int keep_matched_pos;
            regexp_s();
            ~regexp_s();
        } regexp_t;

        typedef spp::sparse_hash_map<std::string, regexp_t *> regexps_t;

        typedef struct property_s {
            char *_string;
            //string  pointer to last matched regexp position
            char *_string_p;
            uint32_t _pos;
            uint32_t _length;
        } property_t;

        class string_ {
            private:
                property_t &p;
            public:
                string_(property_t &p) : p(p) {};
                ~string_() {};
                // FIXME range check and assert on inproperly iteration
                inline char *begin() {return p._string;}
                inline operator char *() {return p._string_p;}
                inline char * operator = (char *a) {
                    assert(a != nullptr and "string is not empty");
                    p._string_p = a; p._pos = 0; p._length = strlen(a); return p._string = a;
                }
                inline operator int() {return p._pos;}
                inline int operator = (int a) {p._string_p = (p._string) + a; return p._pos = a;}
                inline int operator ++(int) {(p._string_p)++; return (p._pos)++;}
                inline int operator --(int) {(p._string_p)--; return (p._pos)--;}
                inline int operator +=(int a) {p._string_p += a; return p._pos += a;}
                inline int operator -=(int a) {p._string_p -= a; return p._pos -= a;}
        };

        class pos_ {
            private:
                property_t &p;
            public:
                pos_(property_t &p) : p(p) {};
                ~pos_() {};
                inline operator int() {return p._pos;}
                inline int operator = (int a) {p._string_p = (p._string) + a; return p._pos = a;}
        };

        class length_ {
            private:
                property_t &p;
            public:
                length_(property_t &p) : p(p) {};
                ~length_() {};
                inline operator int() {return p._length;}

        };

        // list of compiled regexps
        regexps_t   regexps;

        int         *vector;
        const int   vector_size;
        const char  *errstr;
        int         erroff;
        int         matched;
        const char  **match;

        size_t      stat_success;
        size_t      stat_failed;

        property_t  _property;

        regexp_t *compile(const char *_expression);

    public:

        string_ string;
        pos_    pos;
        length_ length;

        Regexp(char *search = nullptr, int vector_size = (10+1)*3); // pcre API possible matches * 3
        ~Regexp();

        int operator()(const std::string &expression, const char *str = nullptr);
        char *operator[](unsigned int i);
        bool exists(unsigned int i);
        int index(unsigned int i, int whence);
        double ratio() {double all = stat_success+stat_failed; return ((double)stat_success/(all ? (double)all : 1));}

};


#endif
