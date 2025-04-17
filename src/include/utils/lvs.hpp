#pragma once

namespace utils {

class lvs {
    private:
        const std::string EMPTY    = "    ";
        const std::string CONTINUE = " │  ";
        const std::string ITEM     = " ├──";
        const std::string LAST     = " └──";
        std::vector<bool> tree;
        int               deep;
        int               prev;
        std::string       out = "";

    public:
        lvs() : prev(0), deep(0){}
        ~lvs(){}
        std::string str(int level, bool last) {
            while (level > deep++ - 1) tree.push_back(false);
            if (tree[level] != (last ? false : true) or prev != level) {
                tree[level] = (last ? false : true);
                prev = level;
                out = "";
                for (int i = 0; i < level; i++) {
                    out += (tree[i] ? CONTINUE : EMPTY);
                }
            }
            return out + (level ? last ? LAST : ITEM : EMPTY);
        }
};

}