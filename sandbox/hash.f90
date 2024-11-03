MODULE hash
    USE iso_c_binding
    IMPLICIT NONE 

    PUBLIC hash_packet
    CONTAINS

    SUBROUTINE hash_packet(packet, hash, length)&
        BIND(c, name="hash_packet")
        INTEGER, INTENT(IN) :: length
        INTEGER :: i
        CHARACTER(c_char), INTENT(IN), DIMENSION(length) :: packet
        CHARACTER(c_char), INTENT(OUT), DIMENSION(8) :: hash

        DO i = 1, length
            hash(i) = 'i'
        END DO

    END SUBROUTINE hash_packet
END MODULE hash
