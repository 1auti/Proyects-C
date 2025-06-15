cmake_minimum_required(VERSION 3.10)
project(implementacion_algoritmos C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Incluir directorios de headers
include_directories(
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/graph
        ${CMAKE_CURRENT_SOURCE_DIR}/algoritmos
        ${CMAKE_CURRENT_SOURCE_DIR}/estructura_datos
        ${CMAKE_CURRENT_SOURCE_DIR}/utils
        ${CMAKE_CURRENT_SOURCE_DIR}/navegacion_gps
        ${CMAKE_CURRENT_SOURCE_DIR}/tests
        ${CMAKE_CURRENT_SOURCE_DIR}/core
        ${CMAKE_CURRENT_SOURCE_DIR}/social_network
        ${CMAKE_CURRENT_SOURCE_DIR}/scheduling
        ${CMAKE_CURRENT_SOURCE_DIR}/pricing
        ${CMAKE_CURRENT_SOURCE_DIR}/realtime
)

# ============================================================================
# VERIFICAR QUE ARCHIVOS EXISTEN ANTES DE INCLUIRLOS
# ============================================================================

# Fuentes de grafos (obligatorias)
set(GRAPH_SOURCES
        graph/graph.c
        graph/lista_adyacente.c
        graph/matrix_adyacente.c
)

# Fuentes de algoritmos (obligatorias)
set(ALGORITHM_SOURCES
        algoritmos/dijkstra.c
        algoritmos/bellman_ford.c
        algoritmos/kruskal.c
        algoritmos/dfs_bfs.c
        algoritmos/cycle_detection.c
)

# Añadir route_planner.c solo si existe
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/algoritmos/route_planner.c")
    list(APPEND ALGORITHM_SOURCES algoritmos/route_planner.c)
    message(STATUS "✅ Incluido: algoritmos/route_planner.c")
else()
    message(STATUS "⚠️  Faltante: algoritmos/route_planner.c")
endif()

# Fuentes de estructuras de datos
set(DATA_STRUCTURE_SOURCES
        estructura_datos/union_find.c
        estructura_datos/priority_queue.c
        estructura_datos/hash_map.c
)

# Fuentes de utilidades
set(UTIL_SOURCES
        utils/file_io.c
        utils/visualization.c
        utils/benchmarks.c
        utils/time_utils.c
)

# Fuentes del core (verificar cada una)
set(CORE_SOURCES)

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/core/transit_system.c")
    list(APPEND CORE_SOURCES core/transit_system.c)
    message(STATUS "✅ Incluido: core/transit_system.c")
else()
    message(STATUS "⚠️  Faltante: core/transit_system.c")
endif()

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/core/station.c")
    list(APPEND CORE_SOURCES core/station.c)
    message(STATUS "✅ Incluido: core/station.c")
else()
    message(STATUS "⚠️  Faltante: core/station.c")
endif()

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/core/route.c")
    list(APPEND CORE_SOURCES core/route.c)
    message(STATUS "✅ Incluido: core/route.c")
else()
    message(STATUS "⚠️  Faltante: core/route.c")
endif()

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/core/line.c")
    list(APPEND CORE_SOURCES core/line.c)
    message(STATUS "✅ Incluido: core/line.c")
else()
    message(STATUS "⚠️  Faltante: core/line.c")
endif()

# Fuentes de red social (opcional)
set(SOCIAL_NETWORK_SOURCES)
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/social_network/social_network.c")
    list(APPEND SOCIAL_NETWORK_SOURCES social_network/social_network.c)
    message(STATUS "✅ Incluido: social_network/social_network.c")
endif()

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/social_network/social_network_examples.c")
    list(APPEND SOCIAL_NETWORK_SOURCES social_network/social_network_examples.c)
    message(STATUS "✅ Incluido: social_network/social_network_examples.c")
endif()

# Fuentes de scheduling (opcional)
set(SCHEDULING_SOURCES)
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/scheduling/schedule.c")
    list(APPEND SCHEDULING_SOURCES scheduling/schedule.c)
    message(STATUS "✅ Incluido: scheduling/schedule.c")
else()
    message(STATUS "⚠️  Faltante: scheduling/schedule.c")
endif()

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/scheduling/timetable.c")
    list(APPEND SCHEDULING_SOURCES scheduling/timetable.c)
    message(STATUS "✅ Incluido: scheduling/timetable.c")
else()
    message(STATUS "⚠️  Faltante: scheduling/timetable.c")
endif()

# Fuentes de pricing (opcional)
set(PRICING_SOURCES)
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/pricing/price_matrix.c")
    list(APPEND PRICING_SOURCES pricing/price_matrix.c)
    message(STATUS "✅ Incluido: pricing/price_matrix.c")
else()
    message(STATUS "⚠️  Faltante: pricing/price_matrix.c")
endif()

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/pricing/fare_calculator.c")
    list(APPEND PRICING_SOURCES pricing/fare_calculator.c)
    message(STATUS "✅ Incluido: pricing/fare_calculator.c")
else()
    message(STATUS "⚠️  Faltante: pricing/fare_calculator.c")
endif()

# Fuentes de tiempo real (opcional)
set(REALTIME_SOURCES)
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/realtime/realtime_data.c")
    list(APPEND REALTIME_SOURCES realtime/realtime_data.c)
    message(STATUS "✅ Incluido: realtime/realtime_data.c")
else()
    message(STATUS "⚠️  Faltante: realtime/realtime_data.c")
endif()

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/realtime/delay_tracker.c")
    list(APPEND REALTIME_SOURCES realtime/delay_tracker.c)
    message(STATUS "✅ Incluido: realtime/delay_tracker.c")
else()
    message(STATUS "⚠️  Faltante: realtime/delay_tracker.c")
endif()

# Fuentes de tests (verificar cada una)
set(TEST_SOURCES)
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/tests/test_dijkstra.c")
    list(APPEND TEST_SOURCES tests/test_dijkstra.c)
endif()

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/tests/test_bellman_ford.c")
    list(APPEND TEST_SOURCES tests/test_bellman_ford.c)
endif()

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/tests/test_kruskal.c")
    list(APPEND TEST_SOURCES tests/test_kruskal.c)
endif()

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/tests/test_traversals.c")
    list(APPEND TEST_SOURCES tests/test_traversals.c)
endif()

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/tests/test_cycles.c")
    list(APPEND TEST_SOURCES tests/test_cycles.c)
endif()

# ============================================================================
# CREAR BIBLIOTECA CON SOLO LOS ARCHIVOS QUE EXISTEN
# ============================================================================

# Verificar que tenemos fuentes suficientes para crear la biblioteca
set(ALL_SOURCES
        ${GRAPH_SOURCES}
        ${ALGORITHM_SOURCES}
        ${DATA_STRUCTURE_SOURCES}
        ${UTIL_SOURCES}
        ${CORE_SOURCES}
        ${SOCIAL_NETWORK_SOURCES}
        ${SCHEDULING_SOURCES}
        ${PRICING_SOURCES}
        ${REALTIME_SOURCES}
        realtime/delay_tracker.h
        core/transit_system_main.c
)

# Solo crear biblioteca si tenemos fuentes
if(ALL_SOURCES)
    add_library(graph_algorithms STATIC ${ALL_SOURCES})
    target_link_libraries(graph_algorithms m)
    message(STATUS "✅ Biblioteca 'graph_algorithms' creada con ${CMAKE_CURRENT_LIST_LENGTH} archivos")
else()
    message(FATAL_ERROR "❌ No se encontraron archivos fuente para crear la biblioteca")
endif()

# ============================================================================
# EJECUTABLES
# ============================================================================

# Ejecutable principal (solo si main.c existe)
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/main.c")
    add_executable(main_program main.c ${TEST_SOURCES})
    target_link_libraries(main_program graph_algorithms m)
    message(STATUS "✅ Ejecutable 'main_program' configurado")
else()
    message(STATUS "⚠️  main.c no encontrado - saltando main_program")
endif()

# Ejecutable GPS (solo si los archivos GPS existen)
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/navegacion_gps/main_gps.c" AND
        EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/navegacion_gps/gps_system.c")
    add_executable(gps_navigator
            navegacion_gps/main_gps.c
            navegacion_gps/gps_system.c
    )
    target_link_libraries(gps_navigator graph_algorithms m)
    message(STATUS "✅ Ejecutable 'gps_navigator' configurado")
else()
    message(STATUS "⚠️  Archivos GPS no encontrados - saltando gps_navigator")
endif()

# ============================================================================
# CONFIGURACIÓN DE COMPILACIÓN
# ============================================================================

# Aplicar flags a todos los targets que existen
set(COMPILE_FLAGS -g -Wall -Wextra)

if(TARGET graph_algorithms)
    target_compile_options(graph_algorithms PRIVATE ${COMPILE_FLAGS})
endif()

if(TARGET main_program)
    target_compile_options(main_program PRIVATE ${COMPILE_FLAGS})
endif()

if(TARGET gps_navigator)
    target_compile_options(gps_navigator PRIVATE ${COMPILE_FLAGS})
endif()

if(TARGET transit_test)
    target_compile_options(transit_test PRIVATE ${COMPILE_FLAGS})
endif()

# ============================================================================
# MENSAJES INFORMATIVOS FINALES
# ============================================================================

message(STATUS "")
message(STATUS "🏗️  ===== RESUMEN DE CONFIGURACIÓN =====")
message(STATUS "📊 Estándar C: ${CMAKE_C_STANDARD}")
message(STATUS "🔧 Tipo de build: ${CMAKE_BUILD_TYPE}")

if(TARGET graph_algorithms)
    message(STATUS "📚 Biblioteca: libgraph_algorithms.a ✅")
else()
    message(STATUS "📚 Biblioteca: ❌ No creada")
endif()

if(TARGET main_program)
    message(STATUS "🚀 main_program: ✅")
else()
    message(STATUS "🚀 main_program: ❌")
endif()

if(TARGET gps_navigator)
    message(STATUS "🗺️  gps_navigator: ✅")
else()
    message(STATUS "🗺️  gps_navigator: ❌")
endif()


message(STATUS "🔗 Matemáticas: enlazadas")
message(STATUS "=====================================")
message(STATUS "")

# Instrucciones de compilación
message(STATUS "📋 Para compilar:")
message(STATUS "   mkdir -p build && cd build")
message(STATUS "   cmake ..")
message(STATUS "   make")
message(STATUS "")