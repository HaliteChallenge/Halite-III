abstract type Entity end

struct Dropoff <: Entity
    owner::Int
    id::Int
    position::Position
end

struct Shipyard <: Entity
    owner::Int
    id::Int
    position::Position
end
spawn(shipyard::Shipyard) = Commands.GENERATE

struct Ship <: Entity
    owner::Int
    id::Int
    position::Position
    halite_amount::Int
end
is_full(ship::Ship) = ship.halite_amount >= Constants.MAX_HALITE
make_dropoff(ship::Ship) = "$(Commands.CONSTRUCT) $(ship.id)"
move(ship::Ship, direction::Direction) = "$(Commands.MOVE) $(ship.id) $(convert(direction))"
stay_still(ship::Ship) = "$(Commands.MOVE) $(ship.id) $(Commands.STAY_STILL)" 

mutable struct Player
    id::Int
    shipyard::Shipyard
    halite_amount::Int
    ships::Dict{Int, Ship}
    dropoffs::Dict{Int, Dropoff}
end
get_ship(player::Player, ship_id::Int) = player.ships[ship_id]
get_ships(player::Player) = collect(values(player.ships))
get_dropoff(player::Player, dropoff_id::Int) = player.dropoffs[dropoff_id]
get_dropoffs(player::Player) = collect(values(player.dropoffs))
has_ship(player::Player, ship_id::Int) = haskey(player.ships, ship_id)
function generate(player_num::Int)
    player, shipyard_x, shipyard_y = parse.(Int, split(readline()))
    Player(player, Shipyard(player, -1, Position(shipyard_x, shipyard_y)), 0, Dict(), Dict())
end
function update!(player::Player, num_ships::Int, num_dropoffs::Int, halite::Int)
    player.ships = Dict(update_ships!(player) for _=1:num_ships)
    player.dropoffs = Dict(update_dropoffs!(player) for _=1:num_dropoffs)
    player.halite_amount = halite
    player
end

mutable struct MapCell
    position::Position
    halite_amount::Int
    ship::Union{Ship, Nothing}
    structure::Union{Entity, Nothing}
end
is_occupied(cell::MapCell) = cell.ship != nothing
has_structure(cell::MapCell) = cell.structure != nothing
is_empty(cell::MapCell) = !is_occupied(cell) && !has_structure(cell) 
structure_type(cell::MapCell) = has_structure(cell) ? typeof(cell.structure) : nothing
mark_unsafe(cell::MapCell, ship::Ship) = MapCell(cell.position, cell.halite_amount, ship, cell.structure)

struct GameMap
    width::Int
    height::Int
    cells::Dict{Position, MapCell}
end
normalize(game_map::GameMap, position::Position) = Position(position.x % game_map.width, position.y % game_map.height)
function calculate_distance(game_map::GameMap, pos_1::Position, pos_2::Position) 
    source = normalize(game_map, pos_1)
    target = normalize(game_map, pos_2)
    resulting_position = abs(source - target)
    min(resulting_position.x, game_map.width - resulting_position.x) + min(resulting_position.y, game_map.height - resulting_position.y)
end
function get_targert_direction(source::Position, target::Position)
    if target.y > source.y
        y = 1
    elseif target.y < source.y 
        y = -1
    else
        y = 0
    end
    if target.x > source.x 
        x = 1
    elseif target.x < source.x 
        x = -1
    else 
        x = 0
    end
    y, x
end

function get_unsafe_moves(game_map::GameMap, source::Position, destination::Position)
    source = normalize(game_map, source)
    destination = normalize(game_map, destination)
    distance = abs(destination - source)
    y_cardinality, x_cardinality = get_targert_direction(source, destination)
    possible_moves = Vector{Int}()
    push!(possible_moves, distance.x < game_map.width / 2 ? x_cardinality : invert(x_cardinality))
    push!(possible_moves, distance.y < game_map.height / 2 ? y_cardinality : invert(y_cardinality))
    return possible_moves
end

function naive_navigate(game_map::GameMap, ship::Ship, destination::Position)
    for target_pos in get_unsafe_moves(game_map, ship, destination)
        if !is_occupied(target_pos)
            mark_unsafe(target_pos, ship)
            return target_pos
        end
    end
    0, 0
end

function generate()
    map_width, map_height = parse.(Int, split(readline()))
    game_map = Dict{Position, MapCell}()
    for y_position in 0:map_height-1
        cells = parse.(Int, split(readline()))
        for x_position in 0:map_width-1
            pos = Position(x_position, y_position)
            i = x_position + 1
            game_map[pos] = MapCell(pos, cells[i], nothing, nothing)
        end
    end
    GameMap(map_width, map_height, game_map)
end

function update!(game_map::GameMap)
    for y in 0:game_map.height-1
        for x in 0:game_map.width-1
            game_map.cells[Position(x, y)].ship = nothing
        end
    end

    for _ in 1:parse(Int, readline())
        cell_x, cell_y, cell_energy = parse.(Int, split(readline()))
        game_map.cells[Position(cell_x, cell_y)].halite_amount = cell_energy
    end
    game_map
end

function update_dropoffs!(player::Player)
    dropoff_id, x_position, y_position = parse.(Int, split(readline()))
    dropoff_id => Dropoff(player.id, dropoff_id, Position(x_position, y_position))
end

function update_ships!(player::Player)
    ship_id, x_position, y_position, halite = parse.(Int, split(readline()))
    ship_id => Ship(player.id, ship_id, Position(x_position, y_position), halite)
end