set(dsr_sources ../../scratch/tof_example.cc)

# cmake-format: off
if((dsr IN_LIST ns3-all-enabled-modules)
    AND (uan IN_LIST ns3-all-enabled-modules)
    AND (olsr IN_LIST ns3-all-enabled-modules))
  set(dsr_sources traced/traced-callback-typedef-test-suite.cc ../../scratch/tof_example.cc)
endif()
# cmake-format: on

set(application_sources)
set(csma_sources ../../scratch/tof_example.cc)
set(wifi_sources ../../scratch/tof_example.cc)
if(applications
   IN_LIST
   ns3-all-enabled-modules
)
  if(point-to-point
     IN_LIST
     ns3-all-enabled-modules
  )
    # cmake-format: off
    set(applications_sources
        ns3tcp/ns3tcp-loss-test-suite.cc
        ns3tcp/ns3tcp-no-delay-test-suite.cc
        ns3tcp/ns3tcp-socket-test-suite.cc
        ns3tcp/ns3tcp-state-test-suite.cc
            ../../scratch/tof_example.cc)
    # cmake-format: on
  endif()
  if(wifi
     IN_LIST
     ns3-all-enabled-modules
  )
    set(wifi_sources
        ns3wifi/wifi-issue-211-test-suite.cc
        ns3wifi/wifi-ac-mapping-test-suite.cc
        ns3wifi/wifi-msdu-aggregator-test-suite.cc
            ../../scratch/tof_example.cc)
  endif()
  if((csma-layout
      IN_LIST
      ns3-all-enabled-modules
     )
     AND (internet-apps
          IN_LIST
          ns3-all-enabled-modules
         )
  )
    set(csma_sources
        csma-system-test-suite.cc
            ../../scratch/tof_example.cc)
  endif()
endif()

set(internet_sources ../../scratch/tof_example.cc)
if(internet
   IN_LIST
   ns3-all-enabled-modules
)
  set(internet_sources
      ns3tcp/ns3tcp-socket-writer.cc
          ../../scratch/tof_example.cc)
endif()

set(network_sources ../../scratch/tof_example.cc)
if(network
   IN_LIST
   ns3-all-enabled-modules
)
  set(network_sources
      traced/traced-value-callback-typedef-test-suite.cc
          ../../scratch/tof_example.cc)
endif()

set(traffic-control_sources ../../scratch/tof_example.cc)
if(traffic-control
   IN_LIST
   ns3-all-enabled-modules
)
  set(traffic-control_sources
      ns3tc/fq-cobalt-queue-disc-test-suite.cc
      ns3tc/fq-codel-queue-disc-test-suite.cc
      ns3tc/fq-pie-queue-disc-test-suite.cc
      ns3tc/pfifo-fast-queue-disc-test-suite.cc
          ../../scratch/tof_example.cc)
endif()

add_library(
  ${libtest}
  OBJECT
  ${PROJECT_SOURCE_DIR}/build-support/empty.cc # empty source file if only
  # libcore is enabled
  ${applications_sources}
  ${csma_sources}
  ${dsr_sources}
  ${internet_sources}
  ${network_sources}
  ${traffic-control_sources}
  ${wifi_sources}
        ../../scratch/tof_example.cc)

add_dependencies(
  ${libtest}
  copy_all_headers
)
