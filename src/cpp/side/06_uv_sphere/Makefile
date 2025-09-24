#### OUTPUT
EXAMPLE_NUMBER = 06_
ARTIFACT = uv_sphere
ARTIFACT_DIR = ../../../../build/modules/${EXAMPLE_NUMBER}${ARTIFACT}
ARTIFACT_BASE = $(ARTIFACT_DIR)/$(ARTIFACT)
ARTIFACT_DATA = $(ARTIFACT_BASE).data
ARTIFACT_WASM = $(ARTIFACT_BASE).wasm
ARTIFACT_JS = $(ARTIFACT_BASE).js
ARTIFACT_HTML = htmls/index.html

DEPLOY_PATH = ../../../../build/webapp_run/modules/${EXAMPLE_NUMBER}${ARTIFACT}/${ARTIFACT}.wasm


### WASM MODULES
MODULES = ./assets/



#### INPUT
GLM_DIR:=./glm
#IMGUI_DIR:=../../../../../imgui
#LODEPNG_DIR=./lodepng
SOURCES = 
SOURCES += ${ARTIFACT}.cpp
#SOURCES += $(LODEPNG_DIR)/lodepng.cpp
#SOURCES += $(IMGUI_DIR)/backends/imgui_impl_glfw.cpp $(IMGUI_DIR)/backends/imgui_impl_opengl3.cpp
#SOURCES += $(IMGUI_DIR)/imgui.cpp $(IMGUI_DIR)/imgui_draw.cpp $(IMGUI_DIR)/imgui_demo.cpp $(IMGUI_DIR)/imgui_widgets.cpp $(IMGUI_DIR)/imgui_tables.cpp

#### FLAGS: LIBS AND HEADERS
LIBS =
LIBS += -lGL
HEADERS =
HEADERS += -I$(GLM_DIR) 
#HEADERS += -I$(IMGUI_DIR) -I$(IMGUI_DIR)/backends
#HEADERS += -I${LODEPNG_DIR}

#### FLAGS: OTHERS
WEBGL_VER = -s USE_WEBGL2=1 -s USE_GLFW=3 -s FULL_ES3=1
USE_WASM = -s WASM=1
EXPORTS = -s EXPORT_ALL=1
DYNLINK_SIDE = -s SIDE_MODULE=1
PRELOADS = --preload-file ./assets
MEM_GROWTH = -sALLOW_MEMORY_GROWTH=1


#### PARAMS
CXX = emcc -std=c++17 -O2
INPUT = $(SOURCES)
OUTPUT = -o $(ARTIFACT_WASM)
FLAGS = $(LIBS) $(HEADERS) $(WEBGL_VER) $(USE_WASM) $(DYNLINK_SIDE) $(EXPORTS) ${MEM_GROWTH}



#### RULES 
all: clean gen_dir side_module 

side_module: 
	$(CXX) $(INPUT) $(OUTPUT) $(FLAGS)

clean:
	rm -rf ${ARTIFACT_DIR} 

gen_dir:
	mkdir ${ARTIFACT_DIR}

deploy:
	cp $(ARTIFACT_WASM) ${DEPLOY_PATH} 