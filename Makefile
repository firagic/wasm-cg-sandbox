#### OUTPUT
ARTIFACT = webapp
ARTIFACT_DIR = build/${ARTIFACT}
MODULES_DIR = build/${ARTIFACT}/modules
ARTIFACT_BASE = $(ARTIFACT_DIR)/app
ARTIFACT_DATA = $(ARTIFACT_BASE).data
ARTIFACT_WASM = $(ARTIFACT_BASE).wasm
ARTIFACT_JS = $(ARTIFACT_BASE).js
ARTIFACT_HTML = resources/htmls/index.html
MODULES_JSON = resources/json/modules.json 



### MODULES
MODULES = --preload-file ./modules
ASSETS = --preload-file ./resources/fonts



#### INPUT
IMGUI_DIR:=../imgui
SOURCES = src/cpp/main/main.cpp src/cpp/main/app.cpp src/cpp/main/gui.cpp src/cpp/main/module.cpp
SOURCES += $(IMGUI_DIR)/backends/imgui_impl_glfw.cpp $(IMGUI_DIR)/backends/imgui_impl_opengl3.cpp
SOURCES += $(IMGUI_DIR)/imgui.cpp $(IMGUI_DIR)/imgui_draw.cpp $(IMGUI_DIR)/imgui_demo.cpp $(IMGUI_DIR)/imgui_widgets.cpp $(IMGUI_DIR)/imgui_tables.cpp


#### FLAGS
LIBS = -lGL
HEADERS = -I$(IMGUI_DIR) -I$(IMGUI_DIR)/backends
WEBGL_VER = -s USE_WEBGL2=1 -s USE_GLFW=3 -s FULL_ES3=1
USE_WASM = -s WASM=1 -s ASSERTIONS=1
DYNLINK_MAIN = -s MAIN_MODULE=1
DYNLINK_SIDE = -s SIDE_MODULE=1
PRELOADS = $(ASSETS) 
MEM_GROWTH = -sALLOW_MEMORY_GROWTH=1

#### PARAMS
CXX = emcc -std=c++17 -O2
INPUT = $(SOURCES)
OUTPUT = -o $(ARTIFACT_JS)
FLAGS = $(LIBS) $(HEADERS) $(WEBGL_VER) $(USE_WASM) $(PRELOADS) $(DYNLINK_MAIN) ${MEM_GROWTH}



#### RULES 
all: clean $(ARTIFACT)

$(ARTIFACT): gen_dir gen_module_dir copy_resources
	$(CXX) $(INPUT) $(OUTPUT) $(FLAGS)

clean:
	rm -rf ${ARTIFACT_DIR} 

run:
	python3 -m http.server -d $(ARTIFACT_DIR)

gen_dir:
	mkdir ${ARTIFACT_DIR}

gen_module_dir:
	mkdir ${MODULES_DIR}

copy_resources:
	cp $(ARTIFACT_HTML) ${ARTIFACT_DIR}
	cp $(MODULES_JSON) $(ARTIFACT_DIR)