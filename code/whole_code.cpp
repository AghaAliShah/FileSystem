#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <ctime>

using namespace std;

class node {
public:
    string name;
    bool isdirectory;
    string content;
    node* firstchild;
    node* nextsibling;
    node* parent;

    time_t createdat;
    time_t modifiedat;
    size_t filesize;
    string owner;
    unsigned int permissions;
    bool issymlink;
    string linktarget;

    node(string n, bool isdir, node* p = nullptr)
        : name(n), isdirectory(isdir), content(""), firstchild(nullptr),
        nextsibling(nullptr), parent(p), filesize(0), owner("root"),
        permissions(0644), issymlink(false), linktarget("") {
        time(&createdat);
        modifiedat = createdat;
    }
};

class filesystem {
private:
    node* root;
    node* current;

    node* find(node* dir, const string& name) {
        node* temp = dir->firstchild;
        while (temp) {
            if (temp->name == name) return temp;
            temp = temp->nextsibling;
        }
        return nullptr;
    }

    vector<string> splitpath(const string& path) {
        vector<string> parts;
        stringstream ss(path);
        string part;
        while (getline(ss, part, '/')) {
            if (!part.empty()) parts.push_back(part);
        }
        return parts;
    }

    node* traverse(const string& path) {
        if (path.empty()) return nullptr;
        node* temp = (path[0] == '/') ? root : current;
        vector<string> parts = splitpath(path);
        for (const string& part : parts) {
            if (part == "..") {
                if (temp->parent) temp = temp->parent;
            }
            else if (part != ".") {
                temp = find(temp, part);
                if (!temp || (!temp->isdirectory && part != parts.back())) return nullptr;
            }
        }
        return temp;
    }

    void serializenode(node* n, ofstream& out) {
        if (!n) return;
        out << n->name << "," << n->isdirectory << "," << n->content << ","
            << n->createdat << "," << n->modifiedat << "," << n->filesize << ","
            << n->owner << "," << n->permissions << "," << n->issymlink << ","
            << n->linktarget << endl;
        serializenode(n->firstchild, out);
        serializenode(n->nextsibling, out);
    }

    node* deserializenode(ifstream& in, node* parent = nullptr) {
        string line;
        if (!getline(in, line)) return nullptr;

        stringstream ss(line);
        string name, content, owner, linktarget;
        bool isdirectory, issymlink;
        time_t createdat, modifiedat;
        size_t filesize;
        unsigned int permissions;

        getline(ss, name, ',');
        ss >> isdirectory;
        ss.ignore();
        getline(ss, content, ',');
        ss >> createdat >> modifiedat >> filesize;
        ss.ignore();
        getline(ss, owner, ',');
        ss >> permissions >> issymlink;
        ss.ignore();
        getline(ss, linktarget, ',');

        node* n = new node(name, isdirectory, parent);
        n->content = content;
        n->createdat = createdat;
        n->modifiedat = modifiedat;
        n->filesize = filesize;
        n->owner = owner;
        n->permissions = permissions;
        n->issymlink = issymlink;
        n->linktarget = linktarget;

        n->firstchild = deserializenode(in, n);
        n->nextsibling = deserializenode(in, parent);
        return n;
    }


    void findindirectory(node* dir, const string& pattern, bool caseinsensitive) {
        if (!dir) return;

        node* temp = dir->firstchild;
        while (temp) {
            if (caseinsensitive ? caseinsensitivematch(temp->name, pattern) : temp->name == pattern) {
                cout << temp->name << endl;
            }
            temp = temp->nextsibling;
        }

        temp = dir->firstchild;
        while (temp) {
            if (temp->isdirectory) {
                findindirectory(temp, pattern, caseinsensitive);
            }
            temp = temp->nextsibling;
        }
    }


    bool caseinsensitivematch(const string& str1, const string& str2) {
        return equal(str1.begin(), str1.end(), str2.begin(), [](char a, char b) {
            return tolower(a) == tolower(b);
            });
    }


public:
    filesystem() {
        root = new node("/", true);
        current = root;
    }

    ~filesystem() {
        vector<node*> stack;
        stack.push_back(root);
        while (!stack.empty()) {
            node* n = stack.back();
            stack.pop_back();
            node* child = n->firstchild;
            while (child) {
                stack.push_back(child);
                child = child->nextsibling;
            }
            delete n;
        }
    }

    void savetofile(const string& filename) {
        ofstream out(filename);
        if (!out) return;
        serializenode(root, out);
        out.close();
    }

    void loadfromfile(const string& filename) {
        ifstream in(filename);
        if (!in) return;
        root = deserializenode(in);
        current = root;
        in.close();
    }

    void makedir(const string& path) {
        vector<string> parts = splitpath(path);
        node* dir = root;

        for (const string& part : parts) {
            node* child = find(dir, part);
            if (!child) {
                child = new node(part, true, dir);
                child->nextsibling = dir->firstchild;
                dir->firstchild = child;
                cout << "Directory created: " << part << endl;
            }
            dir = child;
        }
    }


    void changedir(const string& path) {
        node* dir = traverse(path);
        if (!dir || !dir->isdirectory) return;
        current = dir;
    }

    void printworkingdir() {
        vector<string> path;
        node* temp = current;
        while (temp && temp != root) {
            path.push_back(temp->name);
            temp = temp->parent;
        }
        cout << "/";
        for (auto it = path.rbegin(); it != path.rend(); ++it) {
            cout << *it << "/";
        }
        cout << endl;
    }

    void listdir(const string& path = "") {
        node* dir = path.empty() ? current : traverse(path);
        if (!dir || !dir->isdirectory) {
            cout << "Invalid directory: " << path << endl;
            return;
        }

        node* temp = dir->firstchild;
        while (temp) {
            if (temp->issymlink) {
                cout << temp->name << " -> " << temp->linktarget << " ";
            }
            else {
                cout << temp->name << (temp->isdirectory ? "/" : "") << " ";
            }
            temp = temp->nextsibling;
        }
        cout << endl;
    }




    void createfile(const string& path, const string& content = "") {
        size_t lastslash = path.find_last_of('/');
        string parentpath = (lastslash == 0) ? "/" : path.substr(0, lastslash);
        string name = path.substr(lastslash + 1);

        node* dir = traverse(parentpath);
        if (!dir || !dir->isdirectory) return;
        if (find(dir, name)) return;

        node* newfile = new node(name, false, dir);
        newfile->content = content;
        newfile->filesize = content.size();
        newfile->nextsibling = dir->firstchild;
        dir->firstchild = newfile;
    }

    void readfile(const string& filename) {
        node* file = traverse(filename);
        if (!file) {
            cout << "File not found: " << filename << endl;
            return;
        }

        if (file->issymlink) {
            file = traverse(file->linktarget);
            if (!file) {
                cout << "Broken symbolic link: " << filename << endl;
                return;
            }
        }

        if (file->isdirectory) {
            cout << filename << " is a directory" << endl;
            return;
        }

        cout << file->content << endl;
    }



    void removedir(const string& path) {
        node* parentdir = traverse(path.substr(0, path.find_last_of('/')));
        string dirname = path.substr(path.find_last_of('/') + 1);

        if (!parentdir) return;

        node* prev = nullptr;
        node* temp = parentdir->firstchild;
        while (temp && temp->name != dirname) {
            prev = temp;
            temp = temp->nextsibling;
        }

        if (!temp || !temp->isdirectory) return;

        deletedirectory(temp);

        if (prev) {
            prev->nextsibling = temp->nextsibling;
        }
        else {
            parentdir->firstchild = temp->nextsibling;
        }

        delete temp;
    }

    void deletedirectory(node* dir) {
        if (!dir) return;
        node* child = dir->firstchild;
        while (child) {
            node* nextchild = child->nextsibling;
            if (child->isdirectory) {
                deletedirectory(child);
            }
            delete child;
            child = nextchild;
        }
        dir->firstchild = nullptr;
    }

    void move(const string& sourcepath, const string& destpath) {
        node* sourcenode = traverse(sourcepath);
        if (!sourcenode) return;

        size_t lastslash = destpath.find_last_of('/');
        string destdirpath = (lastslash == 0) ? "/" : destpath.substr(0, lastslash);
        string newname = destpath.substr(lastslash + 1);

        node* destdir = traverse(destdirpath);
        if (!destdir || !destdir->isdirectory) return;

        node* parent = sourcenode->parent;
        if (parent->firstchild == sourcenode) {
            parent->firstchild = sourcenode->nextsibling;
        }
        else {
            node* sibling = parent->firstchild;
            while (sibling && sibling->nextsibling != sourcenode) {
                sibling = sibling->nextsibling;
            }
            if (sibling && sibling->nextsibling == sourcenode) {
                sibling->nextsibling = sourcenode->nextsibling;
            }
        }

        sourcenode->name = newname;
        sourcenode->parent = destdir;
        sourcenode->nextsibling = destdir->firstchild;
        destdir->firstchild = sourcenode;
    }
    void copyfile(node* source, node* destdir) {
        if (!source || !destdir || source->isdirectory || !destdir->isdirectory) return;

        node* newfile = new node(source->name, false, destdir);
        newfile->content = source->content;
        newfile->filesize = source->filesize;
        newfile->owner = source->owner;
        newfile->permissions = source->permissions;
        newfile->createdat = source->createdat;
        newfile->modifiedat = source->modifiedat;
        newfile->issymlink = source->issymlink;
        newfile->linktarget = source->linktarget;

        newfile->nextsibling = destdir->firstchild;
        destdir->firstchild = newfile;
    }

    void copydir(node* source, node* destdir) {
        if (!source || !destdir || !source->isdirectory || !destdir->isdirectory) return;

        node* newdir = new node(source->name, true, destdir);
        newdir->createdat = source->createdat;
        newdir->modifiedat = source->modifiedat;
        newdir->owner = source->owner;
        newdir->permissions = source->permissions;
        newdir->firstchild = nullptr;

        newdir->nextsibling = destdir->firstchild;
        destdir->firstchild = newdir;

        node* child = source->firstchild;
        while (child) {
            if (child->isdirectory) {
                copydir(child, newdir);  // Recursively copy subdirectories
            }
            else {
                copyfile(child, newdir);  // Copy files
            }
            child = child->nextsibling;
        }
    }

    void cp(const string& sourcepath, const string& destpath) {
        node* source = traverse(sourcepath);
        if (!source) {
            cout << "Source not found: " << sourcepath << endl;
            return;
        }

        size_t lastslash = destpath.find_last_of('/');
        string destdirpath = (lastslash == 0) ? "/" : destpath.substr(0, lastslash);
        string destname = destpath.substr(lastslash + 1);

        node* destdir = traverse(destdirpath);
        if (!destdir || !destdir->isdirectory) {
            cout << "Invalid destination directory: " << destdirpath << endl;
            return;
        }

        if (source->isdirectory) {
            copydir(source, destdir);
        }
        else {
            copyfile(source, destdir);
        }

        cout << "Copied " << source->name << " to " << destpath << endl;
    }
    void rename(string oldName, string newName) {
        node* sourceNode = traverse(oldName);
        if (!sourceNode) {
            cout << "No such file or directory: " << oldName << endl;
            return;
        }

        node* destNode = traverse(newName);
        if (destNode) {
            cout << "File or directory already exists: " << newName << endl;
            return;
        }

        sourceNode->name = newName;
        cout << "Renamed " << oldName << " to " << newName << endl;
    }
    void ln(string target, string linkName, bool isSymlink = false) {
        node* targetNode = traverse(target);
        if (!targetNode) {
            cout << "No such file or directory: " << target << endl;
            return;
        }

        node* linkNode = traverse(linkName);
        if (linkNode) {
            cout << "File or directory already exists: " << linkName << endl;
            return;
        }

        node* symlink = new node(linkName, false, current);
        symlink->issymlink = isSymlink;
        symlink->linktarget = target;
        symlink->nextsibling = current->firstchild;
        current->firstchild = symlink;

        cout << "Created symbolic link " << linkName << " -> " << target << endl;
    }

    void stat(const string& path) {
        node* target = traverse(path);
        if (!target) {
            cout << "File or directory not found: " << path << endl;
            return;
        }

        cout << "Name: " << target->name << endl;
        cout << "Type: " << (target->isdirectory ? "Directory" : "File") << endl;
        cout << "Size: " << target->filesize << " bytes" << endl;
        cout << "Owner: " << target->owner << endl;
        cout << "Permissions: " << oct << target->permissions << dec << endl;
        cout << "Created At: " << ctime(&target->createdat);
        cout << "Modified At: " << ctime(&target->modifiedat);
        if (target->issymlink) {
            cout << "Symlink Target: " << target->linktarget << endl;
        }
    }

    void chmod(const string& path, unsigned int mode) {
        node* target = traverse(path);
        if (!target) {
            cout << "File or directory not found: " << path << endl;
            return;
        }

        target->permissions = mode;
        cout << "Permissions updated to " << oct << mode << dec << " for " << path << endl;
    }

    void chown(const string& path, const string& newOwner) {
        node* target = traverse(path);
        if (!target) {
            cout << "File or directory not found: " << path << endl;
            return;
        }

        target->owner = newOwner;
        cout << "Owner updated to " << newOwner << " for " << path << endl;
    }

    bool fileContainsContent(node* file, const string& content) {
        return file->content.find(content) != string::npos;
    }

    void searchInDirectory(node* dir, const string& content) {
        if (!dir) return;

        node* temp = dir->firstchild;
        while (temp) {
            if (!temp->isdirectory && fileContainsContent(temp, content)) {
                cout << "Found in file: " << temp->name << endl;
            }
            temp = temp->nextsibling;
        }

        temp = dir->firstchild;
        while (temp) {
            if (temp->isdirectory) {
                searchInDirectory(temp, content);
            }
            temp = temp->nextsibling;
        }
    }
    void grep(const string& content) {
        searchInDirectory(root, content);
    }

    void executecommand(const string& command) {
        stringstream ss(command);
        string cmd;
        ss >> cmd;

        if (cmd == "mkdir") {
            string path;
            ss >> path;
            makedir(path);
        }
        else if (cmd == "cd") {
            string path;
            ss >> path;
            changedir(path);
        }
        else if (cmd == "pwd") {
            printworkingdir();
        }
        else if (cmd == "ls") {
            string path;
            getline(ss, path);
            if (!path.empty() && path[0] == ' ') path = path.substr(1);
            listdir(path);
        }
        else if (cmd == "touch") {
            string path, content;
            ss >> path;
            if (ss.eof()) {
                createfile(path, "");
            }
            else {
                getline(ss, content);
                createfile(path, content.substr(1));
            }
        }
        else if (cmd == "cat") {
            string filename;
            ss >> filename;
            readfile(filename);
        }
        else if (cmd == "rmdir") {
            string path;
            ss >> path;
            removedir(path);
        }
        else if (cmd == "save") {
            string filename;
            ss >> filename;
            savetofile(filename);
        }
        else if (cmd == "load") {
            string filename;
            ss >> filename;
            loadfromfile(filename);
        }
        else if (cmd == "mv") {
            string source, dest;
            ss >> source >> dest;
            move(source, dest);
        }
        else if (cmd == "find") {
            string pattern;
            bool caseinsensitive = false;
            ss >> pattern;
            string option;
            ss >> option;
            if (option == "-i") caseinsensitive = true;
            findindirectory(root, pattern, caseinsensitive);
        }
        else if (cmd == "cp") {
            string source, dest;
            ss >> source >> dest;
            cp(source, dest);
        }
        else if (cmd == "rename") {
            string oldName, newName;
            ss >> oldName >> newName;
            rename(oldName, newName);
        }
        else if (cmd == "ln") {
            string target, linkName;
            bool isSymlink = false;
            string option;
            ss >> option;
            if (option == "-s") {
                isSymlink = true;
            }
            ss >> target >> linkName;
            ln(target, linkName, isSymlink);
        }
        else if (cmd == "stat") {
            string path;
            ss >> path;
            stat(path);
        }
        else if (cmd == "chmod") {
            string path;
            unsigned int mode;
            ss >> path >> oct >> mode >> dec;
            chmod(path, mode);
        }
        else if (cmd == "chown") {
            string path, newOwner;
            ss >> path >> newOwner;
            chown(path, newOwner);
        }
        else if (cmd == "grep") {
            string content;
            getline(ss, content);
            if (!content.empty() && content[0] == ' ') content = content.substr(1);
            grep(content);
        }
        else if (cmd == "find") {
            string pattern;
            ss >> pattern;

            findindirectory(root, pattern, false);
        }
        else if (cmd == "findi") {
            string pattern;
            ss >> pattern;

            findindirectory(root, pattern, true);
        }


    }


    void startCLI() {
        string command;
        while (true) {
            cout << current->name << "> ";
            getline(cin, command);
            if (command == "exit") break;
            executecommand(command);
        }
    }
};


int main() {
    filesystem fs;
    fs.startCLI();
    return 0;
}


