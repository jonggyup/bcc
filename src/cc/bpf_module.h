/*
 * Copyright (c) 2015 PLUMgrid, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once

#include <stdint.h>
#include <map>
#include <memory>
#include <string>
#include <vector>

namespace llvm {
class ExecutionEngine;
class Function;
class LLVMContext;
class Module;
class Type;
}

namespace ebpf {
class TableDesc;
class BLoader;
class ClangLoader;

class BPFModule {
 private:
  static const std::string FN_PREFIX;
  int init_engine();
  int parse(llvm::Module *mod);
  int finalize();
  int annotate();
  std::unique_ptr<llvm::ExecutionEngine> finalize_reader(std::unique_ptr<llvm::Module> mod);
  int make_reader(llvm::Module *mod, llvm::Type *type);
  void dump_ir(llvm::Module &mod);
  int load_file_module(std::unique_ptr<llvm::Module> *mod, const std::string &file, bool in_memory);
  int load_includes(const std::string &tmpfile);
  int load_cfile(const std::string &file, bool in_memory);
  int kbuild_flags(const char *uname_release, std::vector<std::string> *cflags);
  int run_pass_manager(llvm::Module &mod);
 public:
  BPFModule(unsigned flags);
  ~BPFModule();
  int load_b(const std::string &filename, const std::string &proto_filename);
  int load_c(const std::string &filename);
  int load_string(const std::string &text);
  size_t num_functions() const;
  uint8_t * function_start(size_t id) const;
  uint8_t * function_start(const std::string &name) const;
  const char * function_name(size_t id) const;
  size_t function_size(size_t id) const;
  size_t function_size(const std::string &name) const;
  size_t num_tables() const;
  int table_fd(size_t id) const;
  int table_fd(const std::string &name) const;
  const char * table_name(size_t id) const;
  const char * table_key_desc(size_t id) const;
  const char * table_key_desc(const std::string &name) const;
  size_t table_key_size(size_t id) const;
  size_t table_key_size(const std::string &name) const;
  const char * table_leaf_desc(size_t id) const;
  const char * table_leaf_desc(const std::string &name) const;
  size_t table_leaf_size(size_t id) const;
  size_t table_leaf_size(const std::string &name) const;
  char * license() const;
  unsigned kern_version() const;
 private:
  unsigned flags_;  // 0x1 for printing
  std::string filename_;
  std::string proto_filename_;
  std::unique_ptr<llvm::LLVMContext> ctx_;
  std::unique_ptr<llvm::ExecutionEngine> engine_;
  std::unique_ptr<llvm::Module> mod_;
  std::unique_ptr<BLoader> b_loader_;
  std::unique_ptr<ClangLoader> clang_loader_;
  std::map<std::string, std::tuple<uint8_t *, uintptr_t>> sections_;
  std::unique_ptr<std::map<std::string, TableDesc>> tables_;
  std::vector<std::string> table_names_;
  std::vector<std::string> function_names_;
  std::map<llvm::Type *, llvm::Function *> readers_;
};

}  // namespace ebpf
