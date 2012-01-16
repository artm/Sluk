static const CwiseNullaryOp< ::NormalRnd,Derived>
NormalRnd(Index rows, Index cols, double m, double sd)
{
    return NullaryExpr(rows, cols, ::NormalRnd(m, sd));
}

static const CwiseNullaryOp< ::NormalRnd,Derived>
NormalRnd(Index size, double m, double sd)
{
    return NullaryExpr(size, ::NormalRnd(m, sd));
}

