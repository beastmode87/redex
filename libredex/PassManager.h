/**
 * Copyright (c) 2016-present, Facebook, Inc.
 * All rights reserved.
 *
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree. An additional grant
 * of patent rights can be found in the PATENTS file in the same directory.
 */

#pragma once

#include "Pass.h"
#include "ProguardConfiguration.h"

#include <json/json.h>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

class PassManager {
 public:
  PassManager(const std::vector<Pass*>& passes,
              const Json::Value& config = Json::Value(Json::objectValue),
              bool verify_none_mode = false);

  PassManager(const std::vector<Pass*>& passes,
              const redex::ProguardConfiguration& pg_config,
              const Json::Value& config = Json::Value(Json::objectValue),
              bool verify_none_mode = false);

  struct PassInfo {
    const Pass* pass;
    size_t order; // zero-based
    size_t repeat; // zero-based
    size_t total_repeat;
    std::string name;
    std::unordered_map<std::string, int> metrics;
  };

  void run_passes(DexStoresVector&,
                  const Scope& external_classes,
                  ConfigFiles&);
  void incr_metric(const std::string& key, int value);
  void set_metric(const std::string& key, int value);
  int get_metric(const std::string& key);
  const std::vector<PassManager::PassInfo>& get_pass_info() const;
  const Json::Value& get_config() const { return m_config; }
  bool verify_none_enabled() const { return m_verify_none_mode; }

  // A temporary hack to return the interdex metrics. Will be removed later.
  const std::unordered_map<std::string, int>& get_interdex_metrics();

  redex::ProguardConfiguration& get_proguard_config() { return m_pg_config; }
  bool no_proguard_rules() {
    return m_pg_config.keep_rules.empty() && !m_testing_mode;
  }

  // Call set_testing_mode() in tests that need passes to run which
  // do not use ProGuard configuration keep rules.
  void set_testing_mode() { m_testing_mode = true; }

  const PassInfo* get_current_pass_info() const { return m_current_pass_info; }

 private:
  void activate_pass(const char* name, const Json::Value& cfg);

  void init(const Json::Value& config);

  static void run_type_checker(const Scope& scope, bool verify_moves);

  Json::Value m_config;
  std::vector<Pass*> m_registered_passes;
  std::vector<Pass*> m_activated_passes;

  // Per-pass information and metrics
  std::vector<PassManager::PassInfo> m_pass_info;
  PassInfo* m_current_pass_info;

  redex::ProguardConfiguration m_pg_config;
  bool m_testing_mode;
  bool m_verify_none_mode;
};
