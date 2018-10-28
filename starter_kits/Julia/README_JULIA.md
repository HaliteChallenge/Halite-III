# Uses Julia's packaging system Pkg and project.toml to install packages.

## To add other Julia packages to bot like **Flux**
1. open prompt (bash, cmd, powershell) and navigate to bot folder
2. run `julia --project`
3. type `]` the prompt should now look like `(Halit3) pkg>` 
4. `instantiate`
5. `add Flux`
6. check Project.toml to see if `Flux` has been added to `[deps]` section.  If it has you can now add `using Flux` to **MyBot.jl** and use it. 
7. *Optional*: if you want to use `Flux` in a src/*.jl file add `import Flux` to src/Halite3.jl
8. *Optional*: if you want to add `more.jl` files to src/ make sure add `include("more.jl")` to src/Halite3.jl 
    1. *note: include order matters in Julia*