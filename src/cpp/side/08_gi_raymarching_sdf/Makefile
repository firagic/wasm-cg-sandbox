#### OUTPUT
EXAMPLE_NUMBER = 08_
MODULE_NAME = gi_raymarching_sdf
ARTIFACT = gi_raymarching
ARTIFACT_DIR = ../../../../build/modules/${EXAMPLE_NUMBER}${MODULE_NAME}
ARTIFACT_BASE = $(ARTIFACT_DIR)/$(ARTIFACT)
ARTIFACT_WASM = $(ARTIFACT_BASE).wasm

DEPLOY_DIR = ../../../../build/webapp_run/modules/${EXAMPLE_NUMBER}${MODULE_NAME}
DEPLOY_WASM_PATH = $(DEPLOY_DIR)/$(ARTIFACT).wasm

#### INPUT
GLM_DIR = ../07_gi_raytracer/glm
SOURCES = ${ARTIFACT}.cpp

#### FLAGS
LIBS = -lGL
HEADERS = -I$(GLM_DIR)
WEBGL_VER = -s USE_WEBGL2=1 -s USE_GLFW=3 -s FULL_ES3=1
USE_WASM = -s WASM=1
EXPORTS = -s EXPORT_ALL=1
DYNLINK_SIDE = -s SIDE_MODULE=1
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
	mkdir -p ${ARTIFACT_DIR}

prepare_deploy_dir:
	mkdir -p ${DEPLOY_DIR}

deploy: prepare_deploy_dir
	cp $(ARTIFACT_WASM) $(DEPLOY_WASM_PATH)
	cp module.json $(DEPLOY_DIR)/module.json
	cp gi_raymarching.vs $(DEPLOY_DIR)/gi_raymarching.vs
	cp gi_raymarching.fs $(DEPLOY_DIR)/gi_raymarching.fs
