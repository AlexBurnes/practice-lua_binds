include(download_project)
if (NOT EXISTS ${PROJECT_SOURCE_DIR}/3thd/sparsepp)
    download_project(PROJ sparsepp
        GIT_REPOSITORY      "https://github.com/greg7mdp/sparsepp.git"
        GIT_TAG             "master"
        SOURCE_DIR          "${PROJECT_SOURCE_DIR}/3thd/sparsepp"
        UPDATE_DISCONNECTED 1
    )
endif()
include_directories(${PROJECT_SOURCE_DIR}/3thd/sparsepp)
