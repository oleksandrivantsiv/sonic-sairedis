#include "ClientServerSai.h"

#include "sairedis.h"

#include "swss/logger.h"

#include <gtest/gtest.h>

#include <arpa/inet.h>

using namespace sairedis;

static const char* profile_get_value(
        _In_ sai_switch_profile_id_t profile_id,
        _In_ const char* variable)
{
    SWSS_LOG_ENTER();

    if (variable == NULL)
        return NULL;

    return nullptr;
}

static int profile_get_next_value(
        _In_ sai_switch_profile_id_t profile_id,
        _Out_ const char** variable,
        _Out_ const char** value)
{
    SWSS_LOG_ENTER();

    return 0;
}

static sai_service_method_table_t test_services = {
    profile_get_value,
    profile_get_next_value
};

static const char* client_profile_get_value(
        _In_ sai_switch_profile_id_t profile_id,
        _In_ const char* variable)
{
    SWSS_LOG_ENTER();

    if (variable != NULL && strcmp(variable, SAI_REDIS_KEY_ENABLE_CLIENT) == 0 )
        return "true";
    else
        return NULL;

    return nullptr;
}

static sai_service_method_table_t test_client_services = {
    client_profile_get_value,
    profile_get_next_value
};

TEST(ClientServerSai, ctr)
{
    auto css = std::make_shared<ClientServerSai>();
}

TEST(ClientServerSai, initialize)
{
    auto css = std::make_shared<ClientServerSai>();

    EXPECT_NE(SAI_STATUS_SUCCESS, css->initialize(1, nullptr));

    EXPECT_NE(SAI_STATUS_SUCCESS, css->initialize(0, nullptr));

    EXPECT_EQ(SAI_STATUS_SUCCESS, css->initialize(0, &test_services));

    css = nullptr; // invoke uninitialize in destructor

    css = std::make_shared<ClientServerSai>();

    EXPECT_EQ(SAI_STATUS_SUCCESS, css->initialize(0, &test_services));

    EXPECT_NE(SAI_STATUS_SUCCESS, css->initialize(0, &test_services));
}

TEST(ClientServerSai, objectTypeQuery)
{
    auto css = std::make_shared<ClientServerSai>();

    EXPECT_EQ(SAI_OBJECT_TYPE_NULL, css->objectTypeQuery(0x1111111111111111L));
}

TEST(ClientServerSai, switchIdQuery)
{
    auto css = std::make_shared<ClientServerSai>();

    EXPECT_EQ(SAI_NULL_OBJECT_ID, css->switchIdQuery(0x1111111111111111L));
}

TEST(ClientServerSai, logSet)
{
    auto css = std::make_shared<ClientServerSai>();

    EXPECT_NE(SAI_STATUS_SUCCESS, css->logSet(SAI_API_PORT, SAI_LOG_LEVEL_NOTICE));

    EXPECT_EQ(SAI_STATUS_SUCCESS, css->initialize(0, &test_services));

    EXPECT_EQ(SAI_STATUS_SUCCESS, css->logSet(SAI_API_PORT, SAI_LOG_LEVEL_NOTICE));
}

TEST(ClientServerSai, bulkGetClearStats)
{
    auto css = std::make_shared<ClientServerSai>();

    EXPECT_EQ(SAI_STATUS_FAILURE, css->bulkGetStats(SAI_NULL_OBJECT_ID,
                                                    SAI_OBJECT_TYPE_PORT,
                                                    0,
                                                    nullptr,
                                                    0,
                                                    nullptr,
                                                    SAI_STATS_MODE_BULK_READ,
                                                    nullptr,
                                                    nullptr));

    EXPECT_EQ(SAI_STATUS_FAILURE, css->bulkClearStats(SAI_NULL_OBJECT_ID,
                                                      SAI_OBJECT_TYPE_PORT,
                                                      0,
                                                      nullptr,
                                                      0,
                                                      nullptr,
                                                      SAI_STATS_MODE_BULK_CLEAR,
                                                      nullptr));

    EXPECT_EQ(SAI_STATUS_SUCCESS, css->initialize(0, &test_services));

    EXPECT_EQ(SAI_STATUS_NOT_IMPLEMENTED, css->bulkGetStats(SAI_NULL_OBJECT_ID,
                                                            SAI_OBJECT_TYPE_PORT,
                                                            0,
                                                            nullptr,
                                                            0,
                                                            nullptr,
                                                            SAI_STATS_MODE_BULK_READ,
                                                            nullptr,
                                                            nullptr));

    EXPECT_EQ(SAI_STATUS_NOT_IMPLEMENTED, css->bulkClearStats(SAI_NULL_OBJECT_ID,
                                                              SAI_OBJECT_TYPE_PORT,
                                                              0,
                                                              nullptr,
                                                              0,
                                                              nullptr,
                                                              SAI_STATS_MODE_BULK_CLEAR,
                                                              nullptr));

    css = std::make_shared<ClientServerSai>();
    EXPECT_EQ(SAI_STATUS_SUCCESS, css->initialize(0, &test_client_services));

    EXPECT_EQ(SAI_STATUS_NOT_IMPLEMENTED, css->bulkGetStats(SAI_NULL_OBJECT_ID,
                                                            SAI_OBJECT_TYPE_PORT,
                                                            0,
                                                            nullptr,
                                                            0,
                                                            nullptr,
                                                            SAI_STATS_MODE_BULK_READ,
                                                            nullptr,
                                                            nullptr));

    EXPECT_EQ(SAI_STATUS_NOT_IMPLEMENTED, css->bulkClearStats(SAI_NULL_OBJECT_ID,
                                                              SAI_OBJECT_TYPE_PORT,
                                                              0,
                                                              nullptr,
                                                              0,
                                                              nullptr,
                                                              SAI_STATS_MODE_BULK_CLEAR,
                                                              nullptr));
}

#define TEST_ENTRY(OT,ot)                                                \
TEST(ClientServerSai, OT)                                                \
{                                                                        \
    auto css = std::make_shared<ClientServerSai>();                      \
    sai_ ## ot ## _t e;                                                  \
    EXPECT_EQ(SAI_STATUS_SUCCESS, css->initialize(0, &test_services));   \
    EXPECT_NE(SAI_STATUS_SUCCESS, css->create(&e, 0, nullptr));          \
    EXPECT_NE(SAI_STATUS_SUCCESS, css->set(&e, nullptr));                \
    EXPECT_NE(SAI_STATUS_SUCCESS, css->get(&e, 0, nullptr));             \
    EXPECT_NE(SAI_STATUS_SUCCESS, css->remove(&e));                      \
                                                                         \
    auto ss = std::make_shared<ClientServerSai>();                       \
    EXPECT_EQ(SAI_STATUS_SUCCESS, ss->initialize(0, &test_services));    \
    EXPECT_NE(SAI_STATUS_SUCCESS, ss->create(&e, 0, nullptr));           \
    EXPECT_NE(SAI_STATUS_SUCCESS, ss->set(&e, nullptr));                 \
    EXPECT_NE(SAI_STATUS_SUCCESS, ss->get(&e, 0, nullptr));              \
    EXPECT_NE(SAI_STATUS_SUCCESS, ss->remove(&e));                       \
}

SAIREDIS_DECLARE_EVERY_ENTRY(TEST_ENTRY)

#define TEST_BULK_ENTRY(OT,ot)                                                                                               \
TEST(ClientServerSai, bulk_ ## OT)                                                                                           \
{                                                                                                                            \
    auto css = std::make_shared<ClientServerSai>();                                                                          \
    sai_ ## ot ## _t e[2];                                                                                                   \
    EXPECT_EQ(SAI_STATUS_SUCCESS, css->initialize(0, &test_services));                                                       \
    EXPECT_NE(SAI_STATUS_SUCCESS, css->bulkCreate(0, e, nullptr, nullptr, SAI_BULK_OP_ERROR_MODE_IGNORE_ERROR, nullptr));    \
    EXPECT_NE(SAI_STATUS_SUCCESS, css->bulkSet(2, e, nullptr, SAI_BULK_OP_ERROR_MODE_IGNORE_ERROR, nullptr));                \
    EXPECT_NE(SAI_STATUS_SUCCESS, css->bulkRemove(2, e, SAI_BULK_OP_ERROR_MODE_IGNORE_ERROR, nullptr));                      \
                                                                                                                             \
    auto ss = std::make_shared<ClientServerSai>();                                                                           \
    EXPECT_EQ(SAI_STATUS_SUCCESS, ss->initialize(0, &test_client_services));                                                 \
    EXPECT_NE(SAI_STATUS_SUCCESS, ss->bulkCreate(0, e, nullptr, nullptr, SAI_BULK_OP_ERROR_MODE_IGNORE_ERROR, nullptr));     \
    EXPECT_NE(SAI_STATUS_SUCCESS, ss->bulkSet(0, e, nullptr, SAI_BULK_OP_ERROR_MODE_IGNORE_ERROR, nullptr));                 \
    EXPECT_NE(SAI_STATUS_SUCCESS, ss->bulkRemove(0, e, SAI_BULK_OP_ERROR_MODE_IGNORE_ERROR, nullptr));                       \
}

SAIREDIS_DECLARE_EVERY_BULK_ENTRY(TEST_BULK_ENTRY)
