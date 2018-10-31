"""Add server-side session token

Revision ID: 8cf76fe04ac8
Revises: b56f74b8f3e5
Create Date: 2018-10-22 17:08:54.835148+00:00

"""
from alembic import op
import sqlalchemy as sa
from sqlalchemy.dialects import postgresql


# revision identifiers, used by Alembic.
revision = '8cf76fe04ac8'
down_revision = 'b56f74b8f3e5'
branch_labels = None
depends_on = None


def upgrade():
    op.add_column('user',
        sa.Column('session_secret', sa.String(length=32), nullable=True))


def downgrade():
    op.drop_column('user', 'session_secret')
