#include <gtest/gtest.h>

#include "trading/marketdata/databento/DBNReader.hpp"


TEST(DBNReaderTest, OpenClose)
{
    DBNReader reader;

    const std::string path =
        "00_data/ESU6_2026-06-15_MBO.dbn.zst";

    EXPECT_TRUE(reader.open(path));

    EXPECT_TRUE(reader.is_open());

    reader.close();

    EXPECT_FALSE(reader.is_open());
}
