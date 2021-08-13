include(ExternalProject)

ExternalProject_Add(
  eigen_ext
  PREFIX "vendor/eigen3"
  GIT_REPOSITORY https://gitlab.com/libeigen/eigen.git
  #GIT_TAG        39142904cc2301628931481e8b331cc2d567e22f  # this is "known-to-work", just in case
  CMAKE_ARGS
    -DEIGEN_BUILD_PKGCONFIG=OFF
    -DBUILD_TESTING=OFF
  UPDATE_COMMAND ""
  BUILD_COMMAND ""
  INSTALL_COMMAND ""
  LOG_DOWNLOAD ON   # redirect output to log-file (so that we have less clutter)
  LOG_CONFIGURE ON  # redirect output to log-file (so that we have less clutter)
)

ExternalProject_Get_Property(eigen_ext source_dir)
set(EIGEN3_INCLUDE_DIR ${source_dir})
