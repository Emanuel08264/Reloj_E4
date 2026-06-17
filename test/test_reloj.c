// 1 día.
// Fijar la hora de la alarma y consultarla. Fijar la alarma y avanzar el reloj hasta que suene
// la alarma, deshabilitarla, y avanzar el reloj para que no suene. Hacer sonar la alarma y posponerla 5 min
// y cancelarla hasta el otro día.

#include "unity.h"
#include "reloj.h"

static const hora_t DEFAULT_TIME = {0, 0, 0, 0, 0, 0};
static const hora_t INITIAL_TIME = {0, 2, 1, 2, 1, 2};

#define TICKS_PER_SECOND 3
#define ONE_SECOND       TICKS_PER_SECOND
#define TEN_SECONDS      (10 * ONE_SECOND)
#define ONE_MINUTE       (60 * ONE_SECOND)
#define TEN_MINUTES      (10 * ONE_MINUTE)
#define ONE_HOUR         (60 * ONE_MINUTE)
#define TEN_HOURS        (10 * ONE_HOUR)
#define ONE_DAY          (24 * ONE_HOUR)

void SimulateClockTicks(clock_t reloj, unsigned int ticks) {
    for (unsigned int i = 0; i < ticks; i++) {
        RelojNewTick(reloj);
    }
}

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

// Despues de n ciclos la hora avanza 1 segundo
void test_reloj_avance_un_seg(void) {
    clock_t reloj;
    hora_t hora_actual;
    static const hora_t EXPECTED_TIME = {0, 2, 1, 2, 1, 3};

    reloj = RelojCreate(TICKS_PER_SECOND, NULL);
    (void)RelojSetupCurrentTime(reloj, INITIAL_TIME);
    SimulateClockTicks(reloj, ONE_SECOND);
    RelojGetCurrentTime(reloj, hora_actual);
    TEST_ASSERT_EQUAL_UINT8_ARRAY(EXPECTED_TIME, hora_actual, 6);
}

// Despues de n ciclos la hora avanza 10 segundos
void test_reloj_avance_diez_seg(void) {
    clock_t reloj;
    hora_t hora_actual;
    static const hora_t EXPECTED_TIME = {0, 2, 1, 2, 2, 2};

    reloj = RelojCreate(TICKS_PER_SECOND, NULL);
    (void)RelojSetupCurrentTime(reloj, INITIAL_TIME);
    SimulateClockTicks(reloj, TEN_SECONDS);
    RelojGetCurrentTime(reloj, hora_actual);
    TEST_ASSERT_EQUAL_UINT8_ARRAY(EXPECTED_TIME, hora_actual, 6);
}

// Despues de n ciclos la hora avanza 1 minuto
void test_reloj_avance_un_min(void) {
    clock_t reloj;
    hora_t hora_actual;
    static const hora_t EXPECTED_TIME = {0, 2, 1, 3, 1, 2};

    reloj = RelojCreate(TICKS_PER_SECOND, NULL);
    (void)RelojSetupCurrentTime(reloj, INITIAL_TIME);
    SimulateClockTicks(reloj, ONE_MINUTE);
    RelojGetCurrentTime(reloj, hora_actual);
    TEST_ASSERT_EQUAL_UINT8_ARRAY(EXPECTED_TIME, hora_actual, 6);
}

// Despues de n ciclos la hora avanza 10 minutos
void test_reloj_avance_diez_min(void) {
    clock_t reloj;
    hora_t hora_actual;
    static const hora_t EXPECTED_TIME = {0, 2, 2, 2, 1, 2};

    reloj = RelojCreate(TICKS_PER_SECOND, NULL);
    (void)RelojSetupCurrentTime(reloj, INITIAL_TIME);
    SimulateClockTicks(reloj, TEN_MINUTES);
    RelojGetCurrentTime(reloj, hora_actual);
    TEST_ASSERT_EQUAL_UINT8_ARRAY(EXPECTED_TIME, hora_actual, 6);
}

// Despues de n ciclos la hora avanza 1 hora
void test_reloj_avance_una_hora(void) {
    clock_t reloj;
    hora_t hora_actual;
    static const hora_t EXPECTED_TIME = {0, 3, 1, 2, 1, 2};

    reloj = RelojCreate(TICKS_PER_SECOND, NULL);
    (void)RelojSetupCurrentTime(reloj, INITIAL_TIME);
    SimulateClockTicks(reloj, ONE_HOUR);
    RelojGetCurrentTime(reloj, hora_actual);
    TEST_ASSERT_EQUAL_UINT8_ARRAY(EXPECTED_TIME, hora_actual, 6);
}
// Despues de n ciclos la hora avanza 10 horas
void test_reloj_avance_diez_horas(void) {
    clock_t reloj;
    hora_t hora_actual;
    static const hora_t EXPECTED_TIME = {1, 2, 1, 2, 1, 2};

    reloj = RelojCreate(TICKS_PER_SECOND, NULL);
    (void)RelojSetupCurrentTime(reloj, INITIAL_TIME);
    SimulateClockTicks(reloj, TEN_HOURS);
    RelojGetCurrentTime(reloj, hora_actual);
    TEST_ASSERT_EQUAL_UINT8_ARRAY(EXPECTED_TIME, hora_actual, 6);
}
// Despues de n ciclos la hora avanza 1 día.
void test_reloj_avance_un_dia(void) {
    clock_t reloj;
    hora_t hora_actual;

    reloj = RelojCreate(TICKS_PER_SECOND, NULL);
    (void)RelojSetupCurrentTime(reloj, INITIAL_TIME);
    SimulateClockTicks(reloj, ONE_DAY);
    RelojGetCurrentTime(reloj, hora_actual);
    TEST_ASSERT_EQUAL_UINT8_ARRAY(INITIAL_TIME, hora_actual, 6);
}