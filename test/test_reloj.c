// Despues de n ciclos la hora avanza 1 segundo, 10 segundos, 1 minuto, 10 minutos, 1 hora, 10 horas
// y un dia completo. Fijar la hora de la alarma y consultarla. Fijar la alarma y avanzar el reloj hasta que suene
// la alarma, deshabilitarla, y avanzar el reloj para que no suene. Hacer sonar la alarma y posponerla 5 min
// y cancelarla hasta el otro día.

#include "unity.h"
#include "reloj.h"

static const hora_t DEFAULT_TIME = {0, 0, 0, 0, 0, 0};
static const hora_t INITIAL_TIME = {0, 2, 1, 2, 1, 2};

// Al iniciar el reloj esta en 00:00 y con hora invalida
void test_reloj_inicia_invalido(void) {
    clock_t reloj;
    hora_t hora_actual = {1, 2, 3, 4, 5, 6};

    reloj = RelojCreate(1, NULL);
    TEST_ASSERT_FALSE(RelojGetCurrentTime(reloj, hora_actual));
    TEST_ASSERT_EQUAL_UINT8_ARRAY(DEFAULT_TIME, hora_actual, 6);
}

// Al ajustar la hora el reloj queda en hora y es valida
void test_reloj_ajuste_hora(void) {
    clock_t reloj;
    hora_t hora_actual = {1, 2, 3, 4, 5, 6};

    reloj = RelojCreate(1, NULL);
    TEST_ASSERT_TRUE(RelojSetupCurrentTime(reloj, INITIAL_TIME));
    TEST_ASSERT_TRUE(RelojGetCurrentTime(reloj, hora_actual));
    TEST_ASSERT_EQUAL_UINT8_ARRAY(INITIAL_TIME, hora_actual, 6);
}