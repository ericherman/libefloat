#!/usr/bin/env perl
#
# gen-ints-for-floats.pl - generate ints for rount-triping
# Copyright (C) 2018 Eric Herman
#
# This work is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later
# version.

use strict;
use warnings;

my $float_32 = { exponent_bits => 8,  signifcand_bits => 23, };
my $float_64 = { exponent_bits => 11, signifcand_bits => 52, };

my $float = ( $ARGV[0] eq '64' ) ? $float_64 : $float_32;

my $vals = int_to_float_vals_to_check( $float->{exponent_bits},
    $float->{signifcand_bits} );

for my $val (@$vals) {
    print $val, "\n";
}

exit 0;

sub int_to_float_vals_to_check {
    my ( $significand_bits, $exponent_bits ) = @_;

    # hard-code to the variance to the top 2 and bottom 2 bits always
    my $significands = bitvals_to_test( $significand_bits, 0, 1, 1 );
    my $exponents = bitvals_to_test( $exponent_bits, $significand_bits, 2, 3 );
    my $signs = [ 0, ( 0x01 << ( $significand_bits + $exponent_bits ) ) ];
    my @vals;
    for my $significand (@$significands) {
        for my $exp (@$exponents) {
            for my $sign (@$signs) {
                push @vals, ( $sign + $exp + $significand );
            }
        }
    }
    return [ sort { $a <=> $b } @vals ];
}

sub bitvals_to_test {
    my ( $bits, $shiftup, $lowset, $highset ) = @_;
    $bits ||= 8;
    $shiftup //= 23;
    $lowset  //= 2;
    $highset //= 2;

    my @vals;
    my $mid_ones = 0;
    for ( my $i = 0 ; $i < ( $bits - ( $lowset + $highset ) ) ; ++$i ) {
        $mid_ones |= ( 0x01 << ( $lowset + $i ) );
    }
    for ( my $low = 0 ; $low < 2**$lowset ; ++$low ) {
        for ( my $high = 0 ; $high < ( 2**$highset ) ; ++$high ) {
            my $high_bits = $high << ( $bits - $highset );

            # mid all zeros
            push @vals, ( ( $high_bits + $low ) << $shiftup );

            # mid all ones
            push @vals, ( ( $high_bits + $mid_ones + $low ) << $shiftup );
        }
    }
    return [ sort { $a <=> $b } @vals ];
}
