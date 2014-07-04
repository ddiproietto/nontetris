#ifdef __CHEERP__

/* Cheerp is super-type-safe. We have to provide
 * different functions for each type
 * We use some macro tricks to do that
 */
#define DEFINE_TYPE_SIMPLE(a) DEFINE_TYPE(a,a)

DEFINE_TYPE_SIMPLE(b2ContactPositionConstraint)
DEFINE_TYPE_SIMPLE(b2ContactVelocityConstraint)
DEFINE_TYPE(b2Body *, pb2Body)
DEFINE_TYPE(b2Contact *, pb2Contact)
DEFINE_TYPE(b2Joint *, pb2Joint)
DEFINE_TYPE_SIMPLE(b2Velocity)
DEFINE_TYPE_SIMPLE(b2Position)

#endif
