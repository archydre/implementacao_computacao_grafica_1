cmake_minimum_required(VERSION 3.16)
project(ComputacaoGraficaAvaliacao1 CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Habilitar avisos de compilação sem interromper FreeGLUT
if(MSVC)
    add_compile_options(/W3)
else()
    add_compile_options(-Wall -Wextra -Wpedantic -Wno-deprecated-declarations)
endif()

# Localização de pacotes OpenGL e GLUT
find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)

# Biblioteca utilitária matemática comum
add_library(cg_common
    src/common/math.hpp
    src/common/math.cpp
)
target_include_directories(cg_common PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/common
)

# -------------------------------------------------------------
# Questão 1 — Traçado de Retas
# -------------------------------------------------------------
add_executable(questao1_retas
    src/questao1/main.cpp
)
target_include_directories(questao1_retas PRIVATE
    ${GLUT_INCLUDE_DIRS}
    ${OPENGL_INCLUDE_DIRS}
)
target_link_libraries(questao1_retas PRIVATE
    cg_common
    ${GLUT_LIBRARIES}
    ${OPENGL_LIBRARIES}
)

# -------------------------------------------------------------
# Questão 2 — Transformações Geométricas
# -------------------------------------------------------------
add_executable(questao2_transformacoes
    src/questao2/main.cpp
)
target_include_directories(questao2_transformacoes PRIVATE
    ${GLUT_INCLUDE_DIRS}
    ${OPENGL_INCLUDE_DIRS}
)
target_link_libraries(questao2_transformacoes PRIVATE
    cg_common
    ${GLUT_LIBRARIES}
    ${OPENGL_LIBRARIES}
)

# -------------------------------------------------------------
# Questão 3 — Projeções
# -------------------------------------------------------------
add_executable(questao3_projecoes
    src/questao3/main.cpp
)
target_include_directories(questao3_projecoes PRIVATE
    ${GLUT_INCLUDE_DIRS}
    ${OPENGL_INCLUDE_DIRS}
)
target_link_libraries(questao3_projecoes PRIVATE
    cg_common
    ${GLUT_LIBRARIES}
    ${OPENGL_LIBRARIES}
)

# -------------------------------------------------------------
# Testes Automatizados Matemáticos
# -------------------------------------------------------------
enable_testing()
add_executable(test_matematica
    src/tests/test_math.cpp
)
target_link_libraries(test_matematica PRIVATE
    cg_common
)
add_test(NAME ValidadorMatematico COMMAND test_matematica)
