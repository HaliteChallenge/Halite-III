package hlt

data class EntityId(val id: Int) {
    companion object {
        val NONE = EntityId(-1)
    }

    override fun toString(): String = id.toString()
}
