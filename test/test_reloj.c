#include "unity.h"
#include "reloj.h"
#include <string.h>

static const hora_t DEFAULT_TIME = {0, 0, 0, 0, 0, 0};
static const hora_t DEFAULT_ALARM = {0, 0, 0, 0, 0, 0};
static const hora_t INITIAL_TIME = {0, 2, 1, 2, 1, 2};
static const hora_t ALARM_TIME = {0, 6, 3, 0, 0, 0};
static bool alarma_sono = false;

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

void MockAlarmHandler(clock_t reloj) {
    alarma_sono = true;
}

/**
 * @todo Test cases:
 * - Fijar la hora de la alarma y consultarla.
 * - Fijar la alarma y avanzar el reloj hasta que suene.
 * - Fijar la alarma, deshabilitarla, y avanzar el reloj para que no suene.
 * - Hacer sonar la alarma y posponerla.
 * - Hacer sonar la alarma y cancelarla hasta el otro día.
 * - Probar que el create no devuelve algo nulo.
 * - Probar que no se pone en hora si el argumento es erroneo.
 * - Decidir que sucede con el reloj desconfigurado y el avance de la hora.
 */

/** @test iniciar el reloj esta en 00:00 y con hora invalida */
void test_reloj_inicia_invalido(void) {
    clock_t reloj;
    hora_t hora_actual = {1, 2, 3, 4, 5, 6};

    reloj = RelojCreate(1, NULL);
    TEST_ASSERT_FALSE(RelojGetCurrentTime(reloj, hora_actual));
    TEST_ASSERT_EQUAL_UINT8_ARRAY(DEFAULT_TIME, hora_actual, 6);
}

/** @test Al ajustar la hora el reloj queda en hora y es valida */
void test_reloj_ajuste_hora(void) {
    clock_t reloj;
    hora_t hora_actual = {1, 2, 3, 4, 5, 6};

    reloj = RelojCreate(1, NULL);
    TEST_ASSERT_TRUE(RelojSetupCurrentTime(reloj, INITIAL_TIME));
    TEST_ASSERT_TRUE(RelojGetCurrentTime(reloj, hora_actual));
    TEST_ASSERT_EQUAL_UINT8_ARRAY(INITIAL_TIME, hora_actual, 6);
}

/** @test Despues de n ciclos la hora avanza 1 segundo */
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

/** @test Despues de n ciclos la hora avanza 10 segundos */
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

/** @test Despues de n ciclos la hora avanza 1 minuto */
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

/** @test Despues de n ciclos la hora avanza 10 minutos */
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

/** @test Despues de n ciclos la hora avanza 1 hora */
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
/** @test Despues de n ciclos la hora avanza 10 horas */
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
/** @test Despues de n ciclos la hora avanza 1 día. */
void test_reloj_avance_un_dia(void) {
    clock_t reloj;
    hora_t hora_actual;

    reloj = RelojCreate(TICKS_PER_SECOND, NULL);
    (void)RelojSetupCurrentTime(reloj, INITIAL_TIME);
    SimulateClockTicks(reloj, ONE_DAY);
    RelojGetCurrentTime(reloj, hora_actual);
    TEST_ASSERT_EQUAL_UINT8_ARRAY(INITIAL_TIME, hora_actual, 6);
}

void test_alarma_inicia_deshabilitada() {
    clock_t reloj;
    hora_t hora_alarma = {1, 2, 3, 4, 5, 6};

    reloj = RelojCreate(TICKS_PER_SECOND, NULL);
    TEST_ASSERT_FALSE(RelojGetAlarm(reloj, hora_alarma));
    TEST_ASSERT_EQUAL_UINT8_ARRAY(DEFAULT_ALARM, hora_alarma, 6);
}

/** @test Fijar la hora de la alarma y consultarla.*/
void test_fijar_alarma_y_consultar(void) {
    clock_t reloj;
    hora_t hora_alarma;

    memcpy(hora_alarma, DEFAULT_TIME, sizeof(hora_t));

    reloj = RelojCreate(TICKS_PER_SECOND, NULL);
    (void)RelojSetupCurrentTime(reloj, INITIAL_TIME);

    RelojSetupAlarm(reloj, ALARM_TIME);
    TEST_ASSERT_TRUE(RelojGetAlarm(reloj, hora_alarma));
    TEST_ASSERT_EQUAL_UINT8_ARRAY(ALARM_TIME, hora_alarma, 6);
}

/** @test Fijar la alarma y avanzar el reloj hasta que suene. */
void test_fijar_alarma_y_avanzar_hasta_sonar(void) {
    clock_t reloj;

    alarma_sono = false;

    reloj = RelojCreate(TICKS_PER_SECOND, MockAlarmHandler);
    (void)RelojSetupCurrentTime(reloj, INITIAL_TIME);

    RelojSetupAlarm(reloj, ALARM_TIME);
    SimulateClockTicks(reloj, (4 * ONE_HOUR + 17 * ONE_MINUTE + 48 * ONE_SECOND));

    TEST_ASSERT_TRUE(alarma_sono);
}

/** @test Fijar la alarma, deshabilitarla, y avanzar el reloj para que no suene. */
void test_fijar_alarma_deshabilitar_y_avanzar(void) {
    clock_t reloj;

    alarma_sono = false;

    reloj = RelojCreate(TICKS_PER_SECOND, MockAlarmHandler);
    (void)RelojSetupCurrentTime(reloj, INITIAL_TIME);

    RelojSetupAlarm(reloj, ALARM_TIME);
    RelojToggleAlarm(reloj);
    SimulateClockTicks(reloj, (4 * ONE_HOUR + 17 * ONE_MINUTE + 48 * ONE_SECOND));

    TEST_ASSERT_FALSE(alarma_sono);
}
