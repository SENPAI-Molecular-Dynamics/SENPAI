#include <criterion/criterion.h>
#include <math.h>

#include "vec3.h"
#include "config.h"
#include "util.h"
#include "text.h"

/* Tolerance for floating-point operations */
#define EPSILON 1E-6

/* VEC3_ADD */
Test(vec3_add, addition)
{
    vec3_t v1 = {1.0, 2.0, 3.0};
    vec3_t v2 = {4.0, 5.0, 6.0};
    vec3_t dest;

    vec3_add(&dest, &v1, &v2);

    cr_assert_float_eq(dest.x, 5.0, 1e-6);
    cr_assert_float_eq(dest.y, 7.0, 1e-6);
    cr_assert_float_eq(dest.z, 9.0, 1e-6);
}

Test(vec3_add, zero_vector)
{
    vec3_t v1 = {0.0, 0.0, 0.0};
    vec3_t v2 = {4.5, -2.3, 1.8};
    vec3_t dest;

    vec3_add(&dest, &v1, &v2);

    cr_assert_float_eq(dest.x, 4.5, 1e-6);
    cr_assert_float_eq(dest.y, -2.3, 1e-6);
    cr_assert_float_eq(dest.z, 1.8, 1e-6);
}

Test(vec3_add, negative_numbers)
{
    vec3_t v1 = {-3.2, -1.5, -2.0};
    vec3_t v2 = {-4.8, -3.5, -1.0};
    vec3_t dest;

    vec3_add(&dest, &v1, &v2);

    cr_assert_float_eq(dest.x, -8.0, 1e-6);
    cr_assert_float_eq(dest.y, -5.0, 1e-6);
    cr_assert_float_eq(dest.z, -3.0, 1e-6);
}

/* VEC3_SUB */
Test(vec3_sub, subtraction)
{
    vec3_t v1 = {4.0, 5.0, 6.0};
    vec3_t v2 = {1.0, 2.0, 3.0};
    vec3_t dest;

    vec3_sub(&dest, &v1, &v2);

    cr_assert_float_eq(dest.x, 3.0, 1e-6);
    cr_assert_float_eq(dest.y, 3.0, 1e-6);
    cr_assert_float_eq(dest.z, 3.0, 1e-6);
}

Test(vec3_sub, zero_vector)
{
    vec3_t v1 = {4.5, -2.3, 1.8};
    vec3_t v2 = {0.0, 0.0, 0.0};
    vec3_t dest;

    vec3_sub(&dest, &v1, &v2);

    cr_assert_float_eq(dest.x, 4.5, 1e-6);
    cr_assert_float_eq(dest.y, -2.3, 1e-6);
    cr_assert_float_eq(dest.z, 1.8, 1e-6);
}

Test(vec3_sub, negative_numbers)
{
    vec3_t v1 = {-3.2, -1.5, -2.0};
    vec3_t v2 = {-4.8, -3.5, -1.0};
    vec3_t dest;

    vec3_sub(&dest, &v1, &v2);

    cr_assert_float_eq(dest.x, 1.6, 1e-6);
    cr_assert_float_eq(dest.y, 2.0, 1e-6);
    cr_assert_float_eq(dest.z, -1.0, 1e-6);
}

/* VEC3_MUL */
Test(vec3_mul, multiplication)
{
    vec3_t v = {1.0, 2.0, 3.0};
    vec3_t dest;

    vec3_mul(&dest, &v, 2.0);

    cr_assert_eq(dest.x, 2.0, "Expected dest.x to be 2.0");
    cr_assert_eq(dest.y, 4.0, "Expected dest.y to be 4.0");
    cr_assert_eq(dest.z, 6.0, "Expected dest.z to be 6.0");
}

Test(vec3_mul, multiplication_with_zero)
{
    vec3_t v = {1.0, 2.0, 3.0};
    vec3_t dest;

    vec3_mul(&dest, &v, 0.0);

    cr_assert_eq(dest.x, 0.0, "Expected dest.x to be 0.0");
    cr_assert_eq(dest.y, 0.0, "Expected dest.y to be 0.0");
    cr_assert_eq(dest.z, 0.0, "Expected dest.z to be 0.0");
}

Test(vec3_mul, multiplication_with_negative_lambda)
{
    vec3_t v = {1.0, 2.0, 3.0};
    vec3_t dest;

    vec3_mul(&dest, &v, -2.0);

    cr_assert_eq(dest.x, -2.0, "Expected dest.x to be -2.0");
    cr_assert_eq(dest.y, -4.0, "Expected dest.y to be -4.0");
    cr_assert_eq(dest.z, -6.0, "Expected dest.z to be -6.0");
}

/* VEC3_DIV */
Test(vec3_div, division)
{
    vec3_t v = {4.0, 6.0, 8.0};
    vec3_t dest;

    vec3_div(&dest, &v, 2.0);

    cr_assert_eq(dest.x, 2.0, "Expected dest.x to be 2.0");
    cr_assert_eq(dest.y, 3.0, "Expected dest.y to be 3.0");
    cr_assert_eq(dest.z, 4.0, "Expected dest.z to be 4.0");
}

Test(vec3_div, division_with_zero_lambda)
{
    vec3_t v = {4.0, 6.0, 8.0};
    vec3_t dest;
    void *ret;

    ret = vec3_div(&dest, &v, 0.0);

    // The function is expected to return NULL for failure cases
    cr_assert_null(ret, "Expected dest to be NULL");
}

Test(vec3_div, division_with_small_lambda)
{
    vec3_t v = {4.0, 6.0, 8.0};
    vec3_t dest;
    void *ret;

    ret = vec3_div(&dest, &v, 0.1*DIV_THRESHOLD);

    // The function is expected to return NULL for failure cases
    cr_assert_null(ret, "Expected dest to be NULL");
}

/* VEC3_CROSS */
Test(vec3_cross, cross_product)
{
    vec3_t v1 = {1.0, 2.0, 3.0};
    vec3_t v2 = {4.0, 5.0, 6.0};
    vec3_t dest;

    vec3_cross(&dest, &v1, &v2);

    cr_assert_eq(dest.x, -3.0, "Expected dest.x to be -3.0");
    cr_assert_eq(dest.y, 6.0, "Expected dest.y to be 6.0");
    cr_assert_eq(dest.z, -3.0, "Expected dest.z to be -3.0");
}

/* VEC3_UNIT */
Test(vec3_unit, unit_vector)
{
    vec3_t v = {3.0, 4.0, 0.0};
    vec3_t dest;

    vec3_unit(&dest, &v);

    double magnitude = vec3_mag(&dest);
    cr_assert_float_eq(magnitude, 1.0, 0.00001, "Expected magnitude to be approximately 1.0");
}

Test(vec3_unit, zero_vector)
{
    vec3_t v = {0.0, 0.0, 0.0};
    vec3_t dest;

    vec3_t* result = vec3_unit(&dest, &v);

    // The function is expected to return NULL for failure cases
    cr_assert_null(result, "Expected result to be NULL");
}

/* VEC3_MARSAGLIA */
Test(vec3_marsaglia, unit_sphere)
{
    vec3_t v;
    vec3_marsaglia(&v);

    double magnitude = sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
    cr_assert_float_eq(magnitude, 1.0, 0.00001, "Expected magnitude to be approximately 1.0");
}

Test(vec3_marsaglia, random_direction)
{
    vec3_t v1;
    vec3_t v2;
    vec3_marsaglia(&v1);
    vec3_marsaglia(&v2);

    // Check that the generated vectors are not parallel
    cr_assert_neq(v1.x * v2.y - v2.x * v1.y, 0.0, "Expected the vectors to be non-parallel");
    cr_assert_neq(v1.x * v2.z - v2.x * v1.z, 0.0, "Expected the vectors to be non-parallel");
    cr_assert_neq(v1.y * v2.z - v2.y * v1.z, 0.0, "Expected the vectors to be non-parallel");
}

/* VEC3_DOT */
Test(vec3_dot, dot_product)
{
    vec3_t v1 = {1.0, 2.0, 3.0};
    vec3_t v2 = {4.0, 5.0, 6.0};

    double result = vec3_dot(&v1, &v2);

    cr_assert_eq(result, 32.0, "Expected result to be 32.0");
}

/* VEC3_ANG */
Test(vec3_ang, angle_between_vectors)
{
    vec3_t v1 = {1.0, 0.0, 0.0};
    vec3_t v2 = {0.0, 1.0, 0.0};

    double result = vec3_ang(&v1, &v2);

    cr_assert_float_eq(result, M_PI / 2.0, EPSILON, "Expected result to be approximately pi/2");
}

/* VEC3_MAG */
Test(vec3_mag, magnitude_of_vector)
{
    vec3_t v = {3.0, 4.0, 12.0};

    double result = vec3_mag(&v);

    cr_assert_float_eq(result, 13.0, 1e-6, "Expected result to be approximately 13.0");
}

Test(vec3_mag, magnitude_of_zero_vector)
{
    vec3_t v = {0.0, 0.0, 0.0};

    double result = vec3_mag(&v);

    cr_assert_float_eq(result, 0.0, EPSILON, "Expected result to be 0.0");
}

/* MAT3_TRANSFORM_APPLY */
Test(mat3_transform_apply, apply_transformation_matrix)
{
    mat3_t m = {1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0};
    vec3_t v = {2.0, 3.0, 4.0};

    mat3_transform_apply(&m, &v);

    cr_assert_float_eq(v.x, 2.0, EPSILON, "Expected x-coordinate to remain unchanged");
    cr_assert_float_eq(v.y, 3.0, EPSILON, "Expected y-coordinate to remain unchanged");
    cr_assert_float_eq(v.z, 4.0, EPSILON, "Expected z-coordinate to remain unchanged");
}

Test(mat3_transform_apply, apply_transformation_matrix_non_identity)
{
    mat3_t m = {2.0, 0.0, 0.0, 0.0, 3.0, 0.0, 0.0, 0.0, 4.0};
    vec3_t v = {1.0, 2.0, 3.0};

    mat3_transform_apply(&m, &v);

    cr_assert_float_eq(v.x, 2.0, EPSILON, "Expected x-coordinate to be transformed correctly");
    cr_assert_float_eq(v.y, 6.0, EPSILON, "Expected y-coordinate to be transformed correctly");
    cr_assert_float_eq(v.z, 12.0, EPSILON, "Expected z-coordinate to be transformed correctly");
}

/* MAT3_TRANSFORM_GEN_ROT */
Test(mat3_transform_gen_rot, generate_rotation_matrix)
{
    vec3_t axis = {0.0, 0.0, 1.0};
    double angle = M_PI / 2.0;
    mat3_t m;

    mat3_transform_gen_rot(&m, &axis, angle);

    cr_assert_float_eq(m.x0, 0.0, EPSILON, "Expected x0 element to be approximately 0.0");
    cr_assert_float_eq(m.x1, -1.0, EPSILON, "Expected x1 element to be approximately -1.0");
    cr_assert_float_eq(m.x2, 0.0, EPSILON, "Expected x2 element to be approximately 0.0");
    cr_assert_float_eq(m.y0, 1.0, EPSILON, "Expected y0 element to be approximately 1.0");
    cr_assert_float_eq(m.y1, 0.0, EPSILON, "Expected y1 element to be approximately 0.0");
    cr_assert_float_eq(m.y2, 0.0, EPSILON, "Expected y2 element to be approximately 0.0");
    cr_assert_float_eq(m.z0, 0.0, EPSILON, "Expected z0 element to be approximately 0.0");
    cr_assert_float_eq(m.z1, 0.0, EPSILON, "Expected z1 element to be approximately 0.0");
    cr_assert_float_eq(m.z2, 1.0, EPSILON, "Expected z2 element to be approximately 1.0");
}