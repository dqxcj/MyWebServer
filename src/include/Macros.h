#ifndef SRC_INCLUDE_MACROS_H
#define SRC_INCLUDE_MACROS_H

#define DELETE_COPY(class_name)         \
        class_name(const class_name &)=delete;   \
        class_name &operator=(const class_name &)=delete;

#define DELETE_MOVE(class_name)         \
        class_name(class_name &&)=delete;   \
        class_name &operator=(class_name &&)=delete;

#define DELETE_COPY_AND_MOVE(class_name)        \
        DELETE_COPY(class_name)                 \
        DELETE_MOVE(class_name)

#endif  // SRC_INCLUDE_MACROS_H