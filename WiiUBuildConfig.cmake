# Set configuration values
# Most default values come from SiNG PARTY (USA), change as needed
set(VERSION "0")

# SiNG PARTY (USA) product code, change as needed
set(PRODUCT_CODE "WUP-P-ASWE")

# SiNG PARTY (USA) TID, change as needed
set(TITLE_ID "0005000010101800")
set(OS_VERSION "000500101000400A")
set(BOSS_ID "0000000000000000") # TODO: documentation

# SiNG PARTY (USA) company code, change as needed
set(COMPANY_CODE "0001")

# Long Name appears on the Wii U Menu, Short name appears on the HBM
set(LONG_NAME "Long RIO(e) Name") # Limit: 256 characters
set(SHORT_NAME "Short RIO(e) Name") # Limit: 256 characters
set(PUBLISHER "RIO(e) Publishings") # Limit: 256 characters
set(MASTERING_DATE "2025-1-1 00:00:00") # Format: YYYY-MM-DD HH:MM:SS

# 00000001 - JPN
# 00000002 - USA
# 00000004 - EUR
set(REGION "00000002")

# Common refers to all users on a Wii U system. Account refers to a single account within a Wii U system
# Hex binary, change as needed
set(COMMON_SAVE_SIZE "0000000000000000")
set(COMMON_BOSS_SIZE "0000000000000000")
set(ACCOUNT_SAVE_SIZE "0000000000000000")
set(ACCOUNT_BOSS_SIZE "0000000000000000")

set(JOIN_GAME_ID "00000000") # Relating to nn::fp, specifically nn::fp::GameMode
set(JOIN_GAME_MODE_MASK "0000000000000000") # Relating to nn::fp, specifically nn::fp::GameMode

set(OLV_ACCESS_KEY "0") # Access key for OLV use, nn::olv
set(WOOD_TIN "0")

set(E_MANUAL "0") # E-Manual enabled on HBM
set(DRC_USE "2") # DRC use enabled
set(NETWORK_USE "1") # Network use enabled
set(ONLINE_ACCOUNT_USE "1") # Use of nn::act