package hlt

class Command private constructor(val command: String) {

    companion object {

        fun spawnShip(): Command {
            return Command("g")
        }

        fun transformShipIntoDropoffSite(id: EntityId): Command {
            return Command("c ${id.id}")
        }

        fun move(id: EntityId, direction: Direction): Command {
            return Command("m ${id.id} ${direction.charValue}")
        }
    }
}
