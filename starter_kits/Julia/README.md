Uses Julia's packaging system Pkg. To add other packages to bot
1. clone bot
2. open prompt (bash, cmd, powershell) and navigate to folder
3. run `julia --project`
4. run either
    1. `]` then `add {package name}` 
    2. or `using Pkg` then `Pkg.add("package name")`
5. check Project.toml to see if `package name` has been added to deps section
6. add `using {package name}` to MyBot.jl 