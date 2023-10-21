// Copyright 2023 Google LLC
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

// Swift Package Manager needs at least one source file.

// Is this reproducible if FakeQuery was not C++?
// - Yes, doesn't matter if c++ or objc

// TODO: Is this reproducible in a minimal example?

// THIS IS NEEDED
@import FirebaseFirestoreInternal;
// THIS IS NEEDED
#import "FIRQuery.h"

// THIS ALSO WORKS
//#import <FirebaseFirestoreInternal/FIRQuery.h>

// doesnt work
//#import <FirebaseFirestore/FIRQuery.h>

void foo(void) {
    // THIS IS NEEDED
    Class klass = [FIRQuery class];
}
