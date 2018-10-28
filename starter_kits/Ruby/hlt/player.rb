require 'ship'
require 'dropoff'
require 'shipyard'
require 'position'

# Player object containing all items/metadata pertinent to the player.
class Player
  attr_reader :id, :shipyard, :halite_amount

  def initialize(player_id, shipyard, halite=0)
    @id = player_id
    @shipyard = shipyard
    @halite_amount = halite
    @ships = {}
    @dropoffs = {}
  end

  # Returns a singular ship mapped by the ship id
  # :param ship_id: The ship id of the ship you wish to return
  # :return: the ship object.
  def ship(ship_id)
    @ships[ship_id]
  end

  # :return: Returns all ship objects in a list
  def ships
    @ships.values
  end

  # Returns a singular dropoff mapped by its id
  # :param dropoff_id: The dropoff id to return
  # :return: The dropoff object
  def dropoff(dropoff_id)
    @dropoffs[dropoff_id]
  end

  # :return: Returns all dropoff objects in a list
  def dropoffs
    @dropoffs.values
  end

  # Check whether the player has a ship with a given ID.
  #   Useful if you track ships via IDs elsewhere and want to make
  #   sure the ship still exists.
  # :param ship_id: The ID to check.
  # :return: True if and only if the ship exists.
  def has_ship?(ship_id)
    ship(ship_id) != nil
  end

  # Updates this player object considering the input from the game engine for
  #   the current specific turn.
  # :param game: The game object for extracting ship/dropoff information
  # :param num_ships: The number of ships this player has this turn
  # :param num_dropoffs: The number of dropoffs this player has this turn
  # :param halite: How much halite the player has in total
  # :return: nothing.
  def update(game, num_ships, num_dropoffs, halite)
    @halite_amount = halite

    @ships = {}
    num_ships.times do
      ship = Ship.generate(game, @id)
      @ships[ship.id] = ship
    end

    @dropoffs = {}
    num_dropoffs.times do
      dropoff = Dropoff.generate(game, @id)
      @dropoffs[dropoff.id] = dropoff
    end
  end

  # Creates a player object from the input given by the game engine
  # :return: The player object
  def self.generate(inputs)
    id, x, y = inputs
    return id, Player.new(id, Shipyard.new(id, -1, Position.new(x, y)))
  end
end
