CC = g++
INCLUDE_DIR = -I.. -I../include
V8_INC_ROOT = ../libs/x64.release/obj/
V8_ICU_ROOT = third_party/icu/
V8_INSPECTOR_ROOT = src/inspector/
libv8_names := base libbase external_snapshot libplatform libsampler
libv8_files := $(foreach n, $(libv8_names), $(V8_INC_ROOT)libv8_$(n).a)
icu_names := uc i18n
libicu_files := $(foreach n, $(icu_names), $(V8_INC_ROOT)$(V8_ICU_ROOT)libicu$(n).a)
libinspector_files := $(V8_INC_ROOT)$(V8_INSPECTOR_ROOT)libinspector.a

.PHONY : hello myshell

hello :
	$(call init, hello-world.cc, hello-world)	
	
myshell :
	$(call init, shell.cc, myshellbin)	

define init
	$(CC) $(INCLUDE_DIR)  $(1) -o $(2) -Wl,--start-group \
		$(libv8_files) $(libicu_files) $(libinspector_files) \
		-Wl,--end-group -lrt -ldl -pthread -std=c++0x
endef

clean :
	rm -rf ./*.o hello-world myshellbin process
