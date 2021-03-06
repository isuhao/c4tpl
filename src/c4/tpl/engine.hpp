#ifndef _C4_TPL_ENGINE_HPP_
#define _C4_TPL_ENGINE_HPP_

#include "./token.hpp"

namespace c4 {
namespace tpl {

class Engine
{
public:

    csubstr m_src;
    TokenContainer m_tokens;

public:

    Engine() : m_src(), m_tokens() {}

    bool empty() const { return m_tokens.empty() || m_src.empty(); }
    void clear()
    {
        m_tokens.clear();
    }

    void parse(csubstr src, Rope *rope)
    {
        if(m_tokens.num_pools() == 0)
        {
            register_known_tokens(m_tokens);
        }
        m_src = src;
        clear();
        if(m_src.empty()) return;
        TplLocation pos{rope, {rope->append(src), 0}};
        csubstr rem = m_src;
        while( ! rem.empty())
        {
            auto tk_pos = m_tokens.next_token(&rem, &pos);
            if(tk_pos == NONE) break; // we're done
            auto *tk = m_tokens.get(tk_pos);
            tk->parse(&rem, &pos);
            tk->parse_body(&m_tokens);
        }
    }

    void mark()
    {
        for(auto &tk : m_tokens)
        {
            tk.mark();
        }
    }

    void render(c4::yml::NodeRef & root, Rope *rope) const
    {
        if( ! m_tokens.empty())
        {
            C4_ASSERT(m_tokens.size() == m_tokens.m_token_seq.size());
            Rope const* parsed_rope = m_tokens.get(m_tokens.m_token_seq[0])->rope();
            if(rope != parsed_rope)
            {
                *rope = *parsed_rope;
            }
        }
        for(auto const& token : m_tokens)
        {
            if( ! token.m_root_level) continue;
            token.render(root, rope);
        }
    }

    void render(Tree & t, Rope *r) const
    {
        auto n = t.rootref();
        render(n, r);
    }

    Rope render(c4::yml::NodeRef & root) const
    {
        Rope cp;
        render(root, &cp);
        return cp;
    }

    Rope render(Tree & t) const
    {
        auto n = t.rootref();
        return render(n);
    }

};

} // namespace tpl
} // namespace c4

#endif /* _C4_TPL_ENGINE_HPP_ */
