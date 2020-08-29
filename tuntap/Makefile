CC = gcc
CPPFLAGS =
CFLAGS =
LDFLAGS =
LIBS=

RM = rm -rf
MKDIR = mkdir -p
SED = sed

TARGET = tundemo

DEP_DIR = .dep

SRCS =
OBJS =
DEP_DIRS = $(DEP_DIR)

SRCS += $(wildcard *.c)
OBJS += $(patsubst %.c, %.o, $(SRCS))

TARGET:$(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS) $(LDFLAGS)

$(OBJS):%.o:%.c
	$(CC) -c $(CPPFLAGS) $(CFLAGS) -o $@ $<

sinclude $(patsubst %.c, $(DEP_DIR)/%.d, $(SRCS))
$(DEP_DIR)/%.d:%.c
	-$(MKDIR) $(DEP_DIRS); \
	$(RM) $@; \
	$(CC) -MM $(CPPFLAGS) $< > $@.$$$$; \
	$(SED) 's,\($*\)\.o[: ]*,\1.o $@: ,g' < $@.$$$$ > $@; \
	$(RM) $@.$$$$

.PHONY:fmt clean cleanall

fmt:
	find . -name "*.c" | xargs -I {} indent -kr -i8 {} -o {}

clean:
	-$(RM) $(TARGET) $(OBJS)

cleanall:
	-$(RM) $(TARGET) $(OBJS) $(DEP_DIR) .*.sw?
