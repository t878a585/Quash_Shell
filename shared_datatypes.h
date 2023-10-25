#ifndef SHARED_DATATYPES_H
#define SHARED_DATATYPE_H

struct command {
        char * executable;
        char ** argv;
        int std, sti;   
};

struct commands {
        int count;
        struct command * commands;
};


#endif
