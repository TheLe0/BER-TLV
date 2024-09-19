# Definir a versão mínima do CMake
cmake_minimum_required(VERSION 3.10)

project(BER-TLV C)

# Adicionar a biblioteca
add_library(BER-TLV SHARED src/ber-tlv.c)

# Incluir diretórios de cabeçalhos, se necessário
target_include_directories(BER-TLV PUBLIC ${PROJECT_SOURCE_DIR}/include)
