#### OUTPUT
ARTIFACT = process_models
ARTIFACT_DIR = ../../../../build/modules/${ARTIFACT}
ARTIFACT_BASE = $(ARTIFACT_DIR)/$(ARTIFACT)
ARTIFACT_DATA = $(ARTIFACT_BASE).data
ARTIFACT_WASM = $(ARTIFACT_BASE).wasm
ARTIFACT_JS = $(ARTIFACT_BASE).js
ARTIFACT_HTML = htmls/index.html



### MODULES
MODULES = ./assets/



#### INPUT
GLM_DIR:=./glm
LODEPNG_DIR=./lodepng
SOURCES = process_model.cpp
SOURCES += $(LODEPNG_DIR)/lodepng.cpp


#### FLAGS
LIBS = -lGL
HEADERS = -I$(GLM_DIR) -I${LODEPNG_DIR}
WEBGL_VER = -s USE_WEBGL2=1 -s USE_GLFW=3 -s FULL_ES3=1
USE_WASM = -s WASM=1
EXPORTS = -s EXPORT_ALL=1
DYNLINK_SIDE = -s SIDE_MODULE=1
PRELOADS = --preload-file ./assets
MEM_GROWTH = -sALLOW_MEMORY_GROWTH=1


#### PARAMS
CXX = emcc -std=c++11 -O2
INPUT = $(SOURCES)
OUTPUT = -o $(ARTIFACT_WASM)
FLAGS = $(LIBS) $(HEADERS) $(WEBGL_VER) $(USE_WASM) $(DYNLINK_SIDE) $(EXPORTS) ${MEM_GROWTH}



#### RULES 
all: clean $(ARTIFACT)

$(ARTIFACT): gen_dir
	$(CXX) $(INPUT) $(OUTPUT) $(FLAGS)

clean:
	rm -rf ${ARTIFACT_DIR} 

run:
	python -m http.server -d $(ARTIFACT_DIR)

gen_dir:
	mkdir ${ARTIFACT_DIR}