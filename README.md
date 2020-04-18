# CosMotor
Library for moving a stepper motor following a 1-cos() curve

## Why follow a 1+cos() curve?

The upper plots in the following figure shows the position, velocity and accelerations corresponding
to a movement performed in a 1-cos() way.

The bottom traces correspond to the position and acceleration when the movement is done
with constant acceleration, followed by constant velocity, followed by constant decelaration

![Description](https://github.com/PerePala/CosMotor/blob/master/fig.png)

If you were riding in an elevator following the lower traces your apparent weight would be
changing abruptly: there is jerk. In contrast, the sinusoidal term, being infinitelly derivable,
gives very smooth movements.
