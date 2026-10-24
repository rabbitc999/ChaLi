# Chapter 22 Bytecode deployment

<!-- LANGUAGE NAVIGATION -->
**English** · [简体中文](../22-部署字节码.md)
<!-- END LANGUAGE NAVIGATION -->

Bytecode separates language compilation from execution by a compatible VM host. Export with the `svmb` command and give the resulting artifact set to the matching host. Do not confuse a source file, a textual quadruple dump and a binary `.svmb` container.

```sh
chali svmb hello.chl out build/hello
```

The export prefix identifies related output files. Preserve the files required by the chosen host and export mode rather than assuming that every mode uses one standalone file. The shared host validates container structure and runs through the supported native registry and callback protocol.

Resource budgets are host configuration. A program that exceeds a configured heap budget should fail with a useful diagnostic; increasing a runtime budget does not change the frontend compiler's own memory pool. Architecture and ABI compatibility must be evaluated for the actual deployment artifact and host.

Use bytecode when a shared VM is part of the deployment design. Use AOT when a native application is the desired deliverable. Both paths retain the applicable language semantics and checks.

Exercise: export a small source program, inspect the produced filenames and document the host command used to run it. Next: [compiler commands](23-compiler-commands.md).
