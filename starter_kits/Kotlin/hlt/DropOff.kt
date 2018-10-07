package hlt

class DropOff(owner: PlayerId, id: EntityId, position: Position) : Entity(owner, id, position) {
    companion object {

        internal fun generate(playerId: PlayerId): DropOff {
            val input = Input.readInput()

            val dropOffId = EntityId(input.nextInt)
            val x = input.nextInt
            val y = input.nextInt

            return DropOff(playerId, dropOffId, Position(x, y))
        }
    }
}
