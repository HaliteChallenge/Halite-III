"Base Entity Class from whence Ships, Dropoffs and Shipyards inherit."
abstract type Entity end

"Dropoff class for housing dropoffs."
struct Dropoff <: Entity
    owner::Int
    id::Int
    position::Position
end

"Shipyard class to house shipyards."
struct Shipyard <: Entity
    owner::Int
    id::Int
    position::Position
end

"Return a move to spawn a new ship."
spawn(shipyard::Shipyard) = Commands.GENERATE

"Ship class to house ship entities."
struct Ship <: Entity
    owner::Int
    id::Int
    position::Position
    halite_amount::Int
end

"Is this ship at max halite capacity?"
is_full(ship::Ship, max_halite::Int) = ship.halite_amount >= max_halite

"Return a move to transform this ship into a dropoff."
make_dropoff(ship::Ship) = "$(Commands.CONSTRUCT) $(ship.id)"

"Return a move to move this ship in a direction without checking for collisions."
move(ship::Ship, direction::Direction) = "$(Commands.MOVE) $(ship.id) $(convert(direction))"

"Don't move this ship."
stay_still(ship::Ship) = "$(Commands.MOVE) $(ship.id) $(Commands.STAY_STILL)" 

"Player object containing all items/metadata pertinent to the player."
mutable struct Player
    id::Int
    shipyard::Shipyard
    halite_amount::Int
    ships::Dict{Int, Ship}
    dropoffs::Dict{Int, Dropoff}
end

"""
Returns a singular ship mapped by the ship id.
# Arguments
- `player::Player` : The player whom owns the ship.
- `ship_id::Int` : The ship id of the ship you wish to return.
"""
get_ship(player::Player, ship_id::Int) = player.ships[ship_id]

"Returns all ship objects for given player in an array."
get_ships(player::Player) = collect(values(player.ships))

"Return a random Ship from all ships for given player."
function random_ship(player::Player)
    ships = get_ships(player) 
    if length(ships) == 0
        # TODO: unsafe if player has no ships what to return?
        return @info "Tried to get a random ship but player has no ships."
    else
        return rand(ships)
    end 
end

"""
Returns a singular dropoff mapped by its id.
# Arguments
- `player::Player` : The player whom owns the dropoff.
- `dropoff_id::Int` : The dropoff id to return.
"""
get_dropoff(player::Player, dropoff_id::Int) = player.dropoffs[dropoff_id]

"Returns all dropoff objects for given player in an array."
get_dropoffs(player::Player) = collect(values(player.dropoffs))

"""
Check whether the player has a ship with a given ID. Returns true if and only if the ship exists.
Useful if you track ships via IDs elsewhere and want to make sure the ship still exists.
# Arguments
- `player::Player` : The player whom owns the ship.
- `ship_id::Int` : The ID to check.
"""
has_ship(player::Player, ship_id::Int) = haskey(player.ships, ship_id)

"Creates a player object from the input given by the game engine."
function generate_players()
    player, shipyard_x, shipyard_y = parse.(Int, split(readline()))
    Player(player, Shipyard(player, -1, Position(shipyard_x, shipyard_y)), 0, Dict(), Dict())
end

"""
Updates this player object considering the input from the game engine for the current specific turn.
# Arguments
- `player::Player` : The player to update.
- `num_ships::Int` : The number of ships this player has this turn.
- `num_dropoffs::Int` : The number of dropoffs this player has this turn.
- `halite::Int` : How much halite the player has in total.
"""
function update_player!(player::Player, num_ships::Int, num_dropoffs::Int, halite::Int)
    player.ships = Dict(update_ships!(player) for _=1:num_ships)
    player.dropoffs = Dict(update_dropoffs!(player) for _=1:num_dropoffs)
    player.halite_amount = halite
    player
end

"A cell on the game map."
mutable struct MapCell
    position::Position
    halite_amount::Int
    ship::Union{Ship, Nothing}
    structure::Union{Entity, Nothing}
end

"Return whether this cell has any ships."
is_occupied(cell::MapCell) = cell.ship != nothing

"Return whether this cell has any structures."
has_structure(cell::MapCell) = cell.structure != nothing

"Return when this cell has no ships or structures."
is_empty(cell::MapCell) = !is_occupied(cell) && !has_structure(cell) 

"Return what the structure type in this cell."
structure_type(cell::MapCell) = has_structure(cell) ? typeof(cell.structure) : typeof(nothing)

"""
Mark this cell as unsafe (occupied) for navigation.
Use in conjunction with GameMap.naive_navigate.
"""
function mark_unsafe!(cell::MapCell, ship::Ship)
    cell.ship = ship
    cell
end

"""
The game map.
Can be indexed by a contained entity.
Coordinates start at 0. Coordinates are normalized for you
"""
struct GameMap
    width::Int
    height::Int
    cells::Dict{Position, MapCell}
end

"""
Returns a normalized position object fitting within the bounds of the toroidal map.
i.e.: Takes a point which may or may not be within width and height bounds, and places
it within those bounds considering wraparound.
# Arguments
- `game_map::GameMap` : The game map.
- `position::Position` : A position object.
"""
normalize(game_map::GameMap, position::Position) = Position(mod(position.x, game_map.width), mod(position.y, game_map.height))

# index game_map[x, y] or game_map[Position()] without calling game_map.cells
Base.getindex(G::GameMap, i::Vararg{Int, 2}) = G.cells[normalize(G, Position(i[1], i[2]))]
Base.getindex(G::GameMap, p::Position) = G.cells[normalize(G, p)]

"""
Compute the Manhattan distance between two locations.
Accounts for wrap-around.
# Arguments
- `game_map::GameMap` : The game map.
- `pos_1::Position` : The source from where to calculate.
- `pos_2::Position` : The target to where calculate.
"""
function calculate_distance(game_map::GameMap, pos_1::Position, pos_2::Position) 
    source = normalize(game_map, pos_1)
    target = normalize(game_map, pos_2)
    resulting_position = abs(source - target)
    min(resulting_position.x, game_map.width - resulting_position.x) + min(resulting_position.y, game_map.height - resulting_position.y)
end

"""
Returns where in the cardinality spectrum the target is from source. e.g.: North, East; South, West; etc.
A tuple containing the target Direction. A tuple item (or both) could be None if within same coords
NOTE: Ignores toroid
# Arguments
- `source::Position` : The source position.
- `target::Position` : The target position.
"""
function get_target_direction(source::Position, target::Position)
    y = target.y > source.y ? Direction("South") : target.y < source.y ? Direction("North") : nothing 
    x = target.x > source.x ? Direction("East") : target.x < source.x ? Direction("West") : nothing
    y, x
end

"""
Returns a list of valid (closest) Directions towards your target.
Return the Direction(s) to move closer to the target point, or empty if the points are the same.
This move mechanic does not account for collisions. The multiple directions are if both directional movements
are viable.
# Arguments
- `game_map::GameMap` : The game map.
- `source::Position` : The starting position.
- `destination::Position` : The destination towards which you wish to move your object.
"""
function get_unsafe_moves(game_map::GameMap, source::Position, destination::Position)
    source = normalize(game_map, source)
    destination = normalize(game_map, destination)
    distance = abs(destination - source)
    y_cardinality, x_cardinality = get_target_direction(source, destination)
    x = distance.x < game_map.width / 2 ? x_cardinality : invert(x_cardinality)
    y = distance.y < game_map.height / 2 ? y_cardinality : invert(y_cardinality)
    [x, y]
end

"""
Returns a singular safe move towards the destination.
# Arguments
- `game_map::GameMap` : The game map.
- `ship::Ship` : The ship to move.
- `destination::Position` : Ending position.
"""
function naive_navigate(game_map::GameMap, ship::Ship, destination::Position)
    # No need to normalize destination, since get_unsafe_moves does that.
    for direction in get_unsafe_moves(game_map, ship.position, destination)
        if direction == nothing continue end
        target_pos = normalize(game_map, directional_offset(ship.position, direction))
        if !is_occupied(game_map[target_pos])
            mark_unsafe!(game_map[target_pos], ship)
            return direction
        end
    end
    Direction("Still")
end

"Creates a map object from the input given by the game engine."
function generate_game_map()
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

"Updates the map object from the input given by the game engine."
function update_game_map!(game_map::GameMap)
    for y in 0:game_map.height-1
        for x in 0:game_map.width-1
            game_map[Position(x, y)].ship = nothing
        end
    end

    for _ in 1:parse(Int, readline())
        cell_x, cell_y, cell_energy = parse.(Int, split(readline()))
        game_map[Position(cell_x, cell_y)].halite_amount = cell_energy
    end
    game_map
end

"Updates the dropoff objects Dict from the input given by the game engine for a given player."
function update_dropoffs!(player::Player)
    dropoff_id, x_position, y_position = parse.(Int, split(readline()))
    dropoff_id => Dropoff(player.id, dropoff_id, Position(x_position, y_position))
end

"Updates the ship objects Dict from the input given by the game engine for a given player."
function update_ships!(player::Player)
    ship_id, x_position, y_position, halite = parse.(Int, split(readline()))
    ship_id => Ship(player.id, ship_id, Position(x_position, y_position), halite)
end