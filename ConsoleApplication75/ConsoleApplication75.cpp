#include <iostream>
#include <bitset>

// Definición de constantes
const int EXPONENT_BIAS = 127;
const int SIGNIFICAND_BITS = 23;

// Estructura para representar un número en punto flotante de 32 bits
struct Float32 {
    unsigned int bits; // Patrón de bits completo
    unsigned int sign; // Signo (1 bit)
    unsigned int exponent; // Exponente (8 bits)
    unsigned int significand; // Significando (23 bits)

    // Constructor a partir de un valor entero sin signo
    Float32(unsigned int value) {
        bits = value;
        sign = (bits >> 31) & 1;
        exponent = ((bits >> 23) & 0xFF) - EXPONENT_BIAS;
        significand = (bits & ((1 << 23) - 1)) | (1 << 23); // Normalizar significando
    }

    // Método para obtener el valor decimal
    double to_double() const {
        double result = 0.0;
        if (exponent == -127) { // Valor cero
            return 0.0;
        }
        else if (exponent == 127) { // Valor infinito
            return (sign ? -1.0 : 1.0) * INFINITY;
        }
        else {
            // Calcular la potencia de 2
            double power = 1.0;
            for (int i = 0; i < exponent; ++i) {
                power *= 2.0;
            }

            // Calcular el valor decimal
            result = power * ((significand >> 23) + 1.0);
            if (sign) {
                result *= -1.0;
            }
        }
        return result;
    }

    // Método para imprimir el patrón de bits
    void print_bits() const {
        std::bitset<32> bits_repr(this->bits);
        std::cout << bits_repr << std::endl;
    }
};

// Función para multiplicar dos números en punto flotante
Float32 multiply(const Float32& a, const Float32& b) {
    // Extraer sign, exponent y significando
    unsigned int sign_a = a.sign;
    unsigned int exponent_a = a.exponent;
    unsigned int significand_a = a.significand;

    unsigned int sign_b = b.sign;
    unsigned int exponent_b = b.exponent;
    unsigned int significand_b = b.significand;

    // Multiplicar exponentes
    int new_exponent = exponent_a + exponent_b - EXPONENT_BIAS;

    // Multiplicar significados
    unsigned long long significand_product = (unsigned long long)significand_a * (unsigned long long)significand_b;

    // Normalizar significando
    unsigned int normalized_significand = 0;
    int shift_amount = 0;
    while (significand_product > (1ull << (SIGNIFICAND_BITS + 1))) {
        significand_product >>= 1;
        ++shift_amount;
    }
    normalized_significand = significand_product >> 1;

    // Asignar el bit más significativo como bit de signo del resultado
    unsigned int result_sign = (significand_product >> SIGNIFICAND_BITS) & 1;

    // Aplicar el bias al exponente del resultado
    int result_exponent = new_exponent + EXPONENT_BIAS;

    // Combinar el signo, exponente y significando en un solo patrón de bits
    unsigned int result_bits = (result_sign << 31) | ((result_exponent & 0xFF) << 23) | (normalized_significand & ((1 << SIGNIFICAND_BITS) - 1));

    return Float32(result_bits);
}

int main() {
    // Crear dos números en punto flotante de 32 bits
    Float32 a(1065353216); // Representa el valor 1.0
    Float32 b(1056964608); // Representa el valor 0.5

    // Multiplicar los dos números
    Float32 result = multiply(a, b);

    // Imprimir el resultado
    std::cout << "Resultado de la multiplicación: " << result.to_double() << std::endl;
    std::cout << "Bits del resultado: ";
    result.print_bits();

    return 0;
}
