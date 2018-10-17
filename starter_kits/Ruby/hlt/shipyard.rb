require 'entity'
require 'commands'


# Shipyard class to house shipyards
class Shipyard < Entity
  # Return a move to spawn a new ship.
  def spawn
    Commands::GENERATE
  end
end
