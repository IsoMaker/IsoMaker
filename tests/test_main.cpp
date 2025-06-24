#include <gtest/gtest.h>
#include "../src/Utilities/Vector.hpp"

TEST(SampleTest, BasicAssertions) {
    EXPECT_EQ(1 + 1, 2);  // Exemple simple
}

TEST(Vector2DTest, VectorOperations) {
    // Test vector creation and operators
    Utilities::Vector2D v1(2.0f, 3.0f);
    Utilities::Vector2D v2(4.0f, 1.0f);
    
    // Test addition
    Utilities::Vector2D sum = v1 + v2;
    EXPECT_FLOAT_EQ(sum.x, 6.0f);
    EXPECT_FLOAT_EQ(sum.y, 4.0f);
    
    // Test subtraction
    Utilities::Vector2D diff = v1 - v2;
    EXPECT_FLOAT_EQ(diff.x, -2.0f);
    EXPECT_FLOAT_EQ(diff.y, 2.0f);
    
    // Test scalar multiplication
    Utilities::Vector2D scaled = v1 * 2.0f;
    EXPECT_FLOAT_EQ(scaled.x, 4.0f);
    EXPECT_FLOAT_EQ(scaled.y, 6.0f);
    
    // Test equality operator
    Utilities::Vector2D v3(2.0f, 3.0f);
    EXPECT_TRUE(v1 == v3);
    EXPECT_FALSE(v1 == v2);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
