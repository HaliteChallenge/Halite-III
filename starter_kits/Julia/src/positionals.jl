"Holds positional values (not tuples) in relation to cardinal directions."
struct Direction
    val::String
end

"Returns an array of the cardinal Directions."
get_all_cardinals() = [Direction("North"), Direction("South"), Direction("East"), Direction("West")]

"Choose a random Direction from [North, South, East, West]."
function random_direction()
    rand(get_all_cardinals())
end

"Converts from a Direction value (not tuple) string to the engine's command notation."
function convert(direction::Direction)
    if direction == Direction("North") return Commands.NORTH
    elseif direction == Direction("South") return Commands.SOUTH
    elseif direction == Direction("East") return Commands.EAST
    elseif direction == Direction("West") return Commands.WEST
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
   elseif direction == nothing return nothing
   else throw(DomainError(direction))
   end
end

struct Position 
    x::Int
    y::Int
end

Base.abs(position::Position) = Position(abs(position.x), abs(position.y))
Base.:+(pos_1::Position, pos_2::Position) = Position(pos_1.x + pos_2.x, pos_1.y + pos_2.y)
Base.:-(pos_1::Position, pos_2::Position) = Position(pos_1.x - pos_2.x, pos_1.y - pos_2.y)

"Returns a new position moved in that direction. The position considering a Direction cardinal value."
function directional_offset(position::Position, direction::Direction)
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

"Returns an array of all positions around this specific position in each cardinal direction."
function get_surrounding_cardinals(position::Position)
    [directional_offset(position, direction) for direction in get_all_cardinals()]
end
