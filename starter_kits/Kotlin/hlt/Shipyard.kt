package hlt

class Shipyard(owner: PlayerId, position: Position) : Entity(owner, EntityId.NONE, position) {

    fun spawn(): Command {
        return Command.spawnShip()
    }
}
