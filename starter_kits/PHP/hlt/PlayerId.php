<?php

namespace hlt;

class PlayerId {
    /**
     * @var int
     */
    public $id;

    public function __construct(int $id) {
        $this->id = $id;
    }

    public function __toString()
    {
        return '' . $this->id;
    }

//    @Override
//    public boolean equals(Object o) {
//        if (this == o) return true;
//        if (o == null || getClass() != o.getClass()) return false;
//
//        PlayerId playerId = (PlayerId) o;
//
//        return id == playerId.id;
//    }
//
//    @Override
//    public int hashCode() {
//        return id;
//    }
}
