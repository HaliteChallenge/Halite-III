"Holds positional values (not tuples) in relation to cardinal directions."
struct Direction
    # TODO: test if this is what we want
    val::String
end

"Choose a random Direction from [North, South, East, West]."
Random.rand(rng::Random.AbstractRNG, ::Random.SamplerType{Direction}) = Direction(rand(rng, ["North", "South", "East", "West"]))

"Returns an array of the cardinal Directions."
get_all_cardinals() = [Direction("North"), Direction("South"), Direction("East"), Direction("West")]

"Converts from a Direction value (not tuple) string to the engine's command notation."
function convert(direction::Direction)
    # TODO: test if this is what we want
    if direction == Direction("North") return Commands.NORTH
    elseif direction == Direction("South") return Commands.SOUTH
    elseif direction == Direction("East") return Commands.WEST
    elseif direction == Direction("West") return Commands.EAST
    elseif direction == Direction("Still") return Commands.STAY_STILL
    else throw(DomainError(direction))
    end
 end

"Returns the opposite cardinal direction given a direction."
function invert(direction::Direction)
   if direction == Direction("North") return Direction("South")
   elseif direction == Direction("South") return Direction("North")
   elseif direction == Direction("East") return Direction("West")
   elseif direction == Direction("West") return Direction("East")
   elseif direction == Direction("Still") return Direction("Still")
   else throw(DomainError(direction))
   end
end

# TODO: check whether we need this.
invert(cardinal::Int) = cardinal == 0 ? 0 : cardinal == 1 ? -1 : 1

struct Position 
    x::Int
    y::Int
end

# TODO: check if Position addition is working.
#+(pos_1::Position, pos_2::Position) = (x = pos_1.x + pos_2.x; y = pos_1.y + pos_2.y; Position(x, y))

"Returns a new position moved in that direction. The position considering a Direction cardinal value."
function directional_offset(position::Position, direction::Direction)
    # TODO: test if this is what we want
    if direction == Direction("North") 
        offset = Position(0, -1)
    elseif direction == Direction("South")
        offset = Position(0, 1)
    elseif direction == Direction("East") 
        offset = Position(1, 0)
    elseif direction == Direction("West") 
        offset = Position(-1, 0)
    elseif direction == Direction("Still") 
        offset = Position(0, 0)
    else
        throw(DomainError(direction))
    end
    position + offset
end

"Returns a list of all positions around this specific position in each cardinal direction."
function get_surrounding_cardinals(position::Position)
    [directional_offset(position, Direction.North), directional_offset(position, Direction.South),
     directional_offset(position, Direction.East), directional_offset(position, Direction.West)]
end
