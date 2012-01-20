Thinking aloud

# genes to parameters

genes should be simply sequences of numbers, gene interpreter should
map them to (a) parameres and (b) some parameter range.

# virtual machine

1. what sort of operations are possible? how can they be encoded by a
   string of floats?

one idea is positional code (exactly as it is now), the meaning of a
number depends on its position in the string. the next variation is that
each step of interpretation consumes some portion of the string, length not
known in advance and determined by the interpreter at interpretation
time.

2. should there be several functions - one for generating particles, one
   for advancing, possibly more for some other aspects?

in fact the generalized form of the particle system update can be a
piecewise function of the current life value (or age, shouldn't it be
better called that?)

so... something like:

life < L0:
  life = LF0(...)
  vel = VF0(...)
  pos = PF0(...)
life > L0:
  life -= dt // aging continues after death
life > 0:
  vel = VF1(...)
  pos += vel*dt

in fact above 0 could be more pieces.

to rearrange it'd be something like:

life = Piecewise(life, L0, LF0(...), life - dt)
vel = Piecewise(life, L0, VF0(...), 0, vel, VF1(...) )
pos = Piecewise(life, L0, PF0(...), 0, pos, PF1(...) )

but really, we don't need [L0;0] piece if we don't render points with
life under 0, giving us:

life = Piecewise(life, L0, LF0(...), life - dt)
vel = Piecewise(life, L0, VF0(...), VF1(...) )
pos = Piecewise(life, L0, PF0(...), pos + vel*dt )

but these are all ... case which could be represented as

life = cond(life < L0, Lf0(...), life - dt)
vel = cond(life < L0, VF0(...), VF1(...) )
pos = cond(life < L0, PF0(...), pos + vel*dt )

which in turn looks life we could optimize this by:

c = life < L0;
life = cond( c, LF0(...), life - dt )
vel = cond( c, VF0(...), VF1(...) )
pos = cond( c, PF0(...), pos + vel*dt )

what's left is a cond() array operation. it could be either
(t*F1+(1-t)*F2) or (c ? F1 : F2) depending on complexity of F1 and F2. if
they are simple operations it could be cheaper to do the arithmetic version.


correction: cond(c,e1,e2) actually is c.select(e1, e2), so:

c = life < L0;
life = c.select( LF0(...), life - dt )
vel = c.select( VF0(...), VF1(...) )
pos = c.select( PF0(...), pos + vel*dt )

except in case of vel/pos we need c.replicate<1,3>()...

maybe this could be optimized into a single loop by storing
all parameters in a single matrix and doing a complicated formula, but
for now just do (life < L0) and stuff.

life = (life < L0).select( LF0(...), life - dt )


