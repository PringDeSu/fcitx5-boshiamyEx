# Make sure it produce boshiamyEx.so instead of libboshiamyEx.so
add_library(boshiamyEx SHARED
    state.cpp
    engine.cpp
    enginefactory.cpp
)
target_link_libraries(boshiamyEx PRIVATE Fcitx5::Core)
set_target_properties(boshiamyEx PROPERTIES PREFIX "")
install(TARGETS boshiamyEx DESTINATION "${FCITX_INSTALL_LIBDIR}/fcitx5")

# Addon config file
# We need additional layer of conversion because we want PROJECT_VERSION in it.
configure_file(boshiamyEx-addon.conf.in boshiamyEx-addon.conf)
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/boshiamyEx-addon.conf" RENAME boshiamyEx.conf DESTINATION "${FCITX_INSTALL_PKGDATADIR}/addon")

# Input Method registration file
install(FILES "boshiamyEx.conf" DESTINATION "${FCITX_INSTALL_PKGDATADIR}/inputmethod")

# copy boshiamy.txt to ${FCITX_INSTALL_PKGDATADIR}/boshiamyEx
execute_process(COMMAND mkdir -p ${FCITX_INSTALL_PKGDATADIR}/boshiamyEx)
install(FILES "boshiamy.txt" DESTINATION "${FCITX_INSTALL_PKGDATADIR}/boshiamyEx")

