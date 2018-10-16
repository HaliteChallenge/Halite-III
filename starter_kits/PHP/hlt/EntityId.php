<?php

namespace hlt;

class EntityId {
    /**
     * @var int
     */
    public $id;

    public function __construct(int $id = -1) {
        $this->id = $id;
    }

    public function __toString()
    {
        return '' . $this->id;
    }

//
//    @Override
//    public boolean equals(Object o) {
//        if (this == o) return true;
//        if (o == null || getClass() != o.getClass()) return false;
//
//        EntityId entityId = (EntityId) o;
//
//        return id == entityId.id;
//    }
//
//    @Override
//    public int hashCode() {
//        return id;
//    }
}
