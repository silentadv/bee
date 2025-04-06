# 🐝 Bee

Bee is a compiled language.

## Grammar

$
\begin{align}
\text{prog} \to [stmt]^* \\
\text{stmt} \to
    \begin{cases}
        [exit\_stmt] \\
        [var\_stmt]
    \end {cases}
\\
\text{exit\_stmt} \to \space \text{exit} \space [int\_lit] \space;
\end{align}
$
