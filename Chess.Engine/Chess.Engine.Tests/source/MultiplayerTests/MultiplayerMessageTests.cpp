/*
  ==============================================================================
	Module:			MultiplayerMessageStruct Tests
	Description:    Testing the MultiplayerMessageStruct from the multiplayer module
  ==============================================================================
*/

#include <gtest/gtest.h>

#include "RemoteMessaging/MultiplayerMessageStruct.h"


class MultiplayerMessageStructTests : public ::testing::Test
{
protected:
	void SetUp() override
	{
		// Reset to default state
		testMessage = MultiplayerMessageStruct{};
	}

	MultiplayerMessageStruct testMessage;
};


TEST_F(MultiplayerMessageStructTests, DefaultConstruction)
{
	MultiplayerMessageStruct message;

	EXPECT_EQ(message.type, MultiplayerMessageType::Default) << "Default message type should be Default";
	EXPECT_TRUE(message.data.empty()) << "Default data should be empty";
}


TEST_F(MultiplayerMessageStructTests, CopyConstruction)
{
	// Set up original message
	testMessage.type			  = MultiplayerMessageType::LocalPlayer;
	testMessage.data			  = {0x10, 0x20, 0x30};

	// Create copy
	MultiplayerMessageStruct copy = testMessage;

	EXPECT_EQ(copy.type, testMessage.type) << "Copy should have same type";
	EXPECT_EQ(copy.data, testMessage.data) << "Copy should have same data";

	// Modify copy and ensure original is unchanged
	copy.type = MultiplayerMessageType::PlayerReadyForGameFlag;
	copy.data.push_back(0x40);

	EXPECT_EQ(testMessage.type, MultiplayerMessageType::LocalPlayer) << "Original type should be unchanged";
	EXPECT_EQ(testMessage.data.size(), 3) << "Original data size should be unchanged";
}


TEST_F(MultiplayerMessageStructTests, AssignmentOperator)
{
	// Set up original message
	testMessage.type = MultiplayerMessageType::LocalPlayer;
	testMessage.data = {0x10, 0x20, 0x30};

	// Create another message and assign
	MultiplayerMessageStruct assigned;
	assigned.type = MultiplayerMessageType::Default;
	assigned.data = {0xFF};

	assigned	  = testMessage;

	EXPECT_EQ(assigned.type, testMessage.type) << "Assigned message should have same type";
	EXPECT_EQ(assigned.data, testMessage.data) << "Assigned message should have same data";
}


TEST_F(MultiplayerMessageStructTests, InitializationWithType)
{
	MultiplayerMessageStruct message;
	message.type = MultiplayerMessageType::Move;

	EXPECT_EQ(message.type, MultiplayerMessageType::Move) << "Message type should be set correctly";
	EXPECT_TRUE(message.data.empty()) << "Data should still be empty";
}


TEST_F(MultiplayerMessageStructTests, InitializationWithData)
{
	std::vector<uint8_t> testData = {0x01, 0x02, 0x03, 0x04, 0x05};

	testMessage.type			  = MultiplayerMessageType::ConnectionState;
	testMessage.data			  = testData;

	EXPECT_EQ(testMessage.type, MultiplayerMessageType::ConnectionState) << "Message type should be set correctly";
	EXPECT_EQ(testMessage.data.size(), 5) << "Data size should match input";
	EXPECT_EQ(testMessage.data, testData) << "Data content should match input";
}


TEST_F(MultiplayerMessageStructTests, EmptyDataVector)
{
	testMessage.type = MultiplayerMessageType::Chat;
	testMessage.data = std::vector<uint8_t>{};

	EXPECT_EQ(testMessage.type, MultiplayerMessageType::Chat) << "Message type should be set";
	EXPECT_TRUE(testMessage.data.empty()) << "Empty data vector should be handled correctly";
}


TEST_F(MultiplayerMessageStructTests, LargeDataVector)
{
	// Create a large data vector
	std::vector<uint8_t> largeData(1024, 0xAA);

	testMessage.type = MultiplayerMessageType::Move;
	testMessage.data = largeData;

	EXPECT_EQ(testMessage.type, MultiplayerMessageType::Move) << "Message type should be set";
	EXPECT_EQ(testMessage.data.size(), 1024) << "Large data vector should be handled correctly";

	// Verify all bytes are correct
	for (size_t i = 0; i < testMessage.data.size(); ++i)
	{
		EXPECT_EQ(testMessage.data[i], 0xAA) << "Data at index " << i << " should be correct";
	}
}


TEST_F(MultiplayerMessageStructTests, BinaryDataHandling)
{
	// Test with various binary data patterns
	std::vector<uint8_t> binaryData = {
		0x00, 0xFF, 0x55, 0xAA, // Pattern: null, max, alternating bits
		0x01, 0x02, 0x04, 0x08, // Power of 2 pattern
		0x10, 0x20, 0x40, 0x80	// More power of 2 pattern
	};

	testMessage.type = MultiplayerMessageType::InvitationRequest;
	testMessage.data = binaryData;

	EXPECT_EQ(testMessage.data.size(), binaryData.size()) << "Binary data size should be preserved";

	for (size_t i = 0; i < binaryData.size(); ++i)
	{
		EXPECT_EQ(testMessage.data[i], binaryData[i]) << "Binary data at index " << i << " should be preserved";
	}
}


TEST_F(MultiplayerMessageStructTests, DataVectorOperations)
{
	testMessage.type = MultiplayerMessageType::Move;

	// Test push_back
	testMessage.data.push_back(0x01);
	testMessage.data.push_back(0x02);

	EXPECT_EQ(testMessage.data.size(), 2) << "Data should have 2 elements after push_back operations";
	EXPECT_EQ(testMessage.data[0], 0x01) << "First element should be correct";
	EXPECT_EQ(testMessage.data[1], 0x02) << "Second element should be correct";

	// Test clear
	testMessage.data.clear();
	EXPECT_TRUE(testMessage.data.empty()) << "Data should be empty after clear";

	// Test resize
	testMessage.data.resize(5, 0xCC);
	EXPECT_EQ(testMessage.data.size(), 5) << "Data should have 5 elements after resize";
	for (size_t i = 0; i < testMessage.data.size(); ++i)
	{
		EXPECT_EQ(testMessage.data[i], 0xCC) << "Element " << i << " should have correct value after resize";
	}
}


TEST_F(MultiplayerMessageStructTests, MessageTypeEnumValues)
{
	// Test that we can assign all expected message types
	std::vector<MultiplayerMessageType> types = {MultiplayerMessageType::Default,
												 MultiplayerMessageType::Move,
												 MultiplayerMessageType::Chat,
												 MultiplayerMessageType::ConnectionState,
												 MultiplayerMessageType::InvitationRequest,
												 MultiplayerMessageType::InvitationResponse,
												 MultiplayerMessageType::LocalPlayer,
												 MultiplayerMessageType::PlayerReadyForGameFlag};

	for (auto type : types)
	{
		testMessage.type = type;
		EXPECT_EQ(testMessage.type, type) << "Should be able to set message type correctly";
	}
}



